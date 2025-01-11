#include "Crv/Pch.h"
#include "Crv/Renderers/HelloTriangleRenderer.h"

#include <algorithm>

#include "Crv/Data/StaticModelCreateInfo.h"
#include "Crv/PrivateData/StaticModel.h"
#include "Crv/PrivateData/DataGenerators.h"

#include "Snd/ShaderCode.h"
#include "Snd/Dx12/Context.h"
#include "Snd/Dx12/RootSignature.h"
#include "Snd/Dx12/PipelineDescriptor.h"
#include "Snd/Dx12/PipelineState.h"
#include "Snd/Dx12/DescriptorHeap.h"
#include "Snd/Dx12/Texture.h"

#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/StaticSamplers.h"
#include "Snd/Dx12/Utils/Common.h"

#include "PxHelloTriangle.h"
#include "VxHelloTriangle.h"

namespace Crv
{
	HelloTriangleRenderer::HelloTriangleRenderer(const HWND& hwnd, const UINT displayWidth, const UINT displayHeight)
		: m_hwnd{ hwnd }, m_displayWidth{ displayWidth }, m_displayHeight{ displayHeight }
	{
	}

	HelloTriangleRenderer::~HelloTriangleRenderer() = default;

	void HelloTriangleRenderer::init()
	{
		m_context = std::make_shared<Snd::Dx12::Context>(m_hwnd, m_displayWidth, m_displayHeight);

		// cbv and srv preparation
		{
			m_inputDescriptorHeap = std::make_unique<Snd::Dx12::DescriptorHeap>(
			"HelloTriangle input Descriptor",
			   *m_context->getDevice(),
			   Snd::Dx12::DescriptorHeapType::CbvSrvUav,
			   Snd::Dx12::Context::getFrameCount() + 1, // multiple constant buffers for multiple buffering and one for texture
			   true);


			m_constantBufferCpuVisibleGpuPointers.resize(Snd::Dx12::Context::getFrameCount());
			m_constantBufferGpuData.resize(Snd::Dx12::Context::getFrameCount());

			for (int i = 0; i < Snd::Dx12::Context::getFrameCount(); ++i)
			{
				m_constantBufferGpuData[i] = std::make_shared<Snd::Dx12::Buffer>(
					"ConstantBuffer",
					*m_context->getDevice(),
					Snd::Dx12::ResourceHeapType::Upload,
					Snd::Dx12::ResourceFlag::None,
					Snd::Dx12::ResourceState::Common,
					sizeof(SceneConstantBuffer), 1);

				m_constantBufferGpuData[i]->map(0, 0, reinterpret_cast<void**>(&m_constantBufferCpuVisibleGpuPointers[i]));
				memcpy(m_constantBufferCpuVisibleGpuPointers[i], &m_constantBufferCpuData, sizeof(m_constantBufferCpuData));
				m_constantBufferGpuData[i]->unmap();

				m_inputDescriptorHeap->newConstantBufferView(*m_context->getDevice(), *m_constantBufferGpuData[i], i);
			}

			m_cpuTextureData = Generator::generateTextureData(256, 256, 4);
			m_gpuTexture = std::make_shared<Snd::Dx12::Texture2D>(
				"ChessBoard",
				*m_context->getDevice(),
				Snd::Dx12::ResourceHeapType::Default,
				Snd::Dx12::ResourceFlag::None,
				256, 256,
				Snd::Dx12::TextureFormat::R8G8B8A8Unorm,
				Snd::Dx12::ResourceState::CopyDest);

			m_context->upload(m_gpuTexture, m_cpuTextureData.data());

			m_context->transition(m_gpuTexture, Snd::Dx12::ResourceState::PixelShaderResource);

			m_inputDescriptorHeap->newShaderResourceView(*m_context->getDevice(), *m_gpuTexture, 2);
		}

		// root signature creation
		{
			const std::vector inputTableParameterCbv
			{
				Snd::Dx12::DescriptorTableParameter{Snd::Dx12::DescriptorTableParameterType::Cbv, 1, 0},
			};

			const std::vector inputTableParameterSrv
			{
				Snd::Dx12::DescriptorTableParameter{Snd::Dx12::DescriptorTableParameterType::Srv, 1, 0},
			};

			std::vector rootParameters
			{
				Snd::Dx12::RootParameter{Snd::Dx12::RootParameterType::Table, Snd::Dx12::ShaderVisibility::Vertex, inputTableParameterCbv},
				Snd::Dx12::RootParameter{Snd::Dx12::RootParameterType::Table, Snd::Dx12::ShaderVisibility::Pixel, inputTableParameterSrv}
			};

			Snd::Dx12::StaticSamplers samplers;
			samplers.add(0, 0, Snd::Dx12::ShaderVisibility::Pixel);

			m_helloTriangleRootSignature = std::make_unique<Snd::Dx12::RootSignature>(
				"HelloTriangle RootSignature",
				*m_context->getDevice(),
				samplers,
				rootParameters,
				Snd::Dx12::RootSignatureFlag::AllowInputAssembler);
		}

		// pipeline definition
		{
			const std::vector inputAssemblyParameters
		   {
		   		Snd::InputParameter{ "POSITION", Snd::InputParameterType::Vector4, 0, 0, Snd::InputRate::PerVertex },
		   		Snd::InputParameter{ "TEXCOORD", Snd::InputParameterType::Vector2, 16, 0, Snd::InputRate::PerVertex },
		   };

			constexpr Snd::ShaderCode vertexCode{ VxHelloTriangle, sizeof(VxHelloTriangle) };
			constexpr Snd::ShaderCode pixelCode{ PxHelloTriangle, sizeof(PxHelloTriangle) };

			Snd::Dx12::ClassicGraphicsPipelineDescriptor graphicsPipelineDesc
			{
				*m_helloTriangleRootSignature,
				vertexCode,
				pixelCode,
				inputAssemblyParameters
			};

			m_helloTrianglePipeline = std::make_unique<Snd::Dx12::PipelineState>("HelloTriangle PipelineState", *m_context->getDevice(), graphicsPipelineDesc);
		}

		// triangle mesh preparation
		{
			const float aspectRatio = static_cast<float>(m_displayWidth) / static_cast<float>(m_displayHeight);
			SquareVertex squareVertices[] =
			{
				{ { -0.25f, 0.25f * aspectRatio, 0.0f, 1.0f }, { 0.0f, 0.0f } },
				{ {  0.25f, 0.25f * aspectRatio, 0.0f, 1.0f }, { 1.0f, 0.0f } },
				{ {  0.25f, -0.25f * aspectRatio, 0.0f, 1.0f }, { 1.0f, 1.0f } },
				{ { -0.25f, -0.25f * aspectRatio, 0.0f, 1.0f }, { 0.0f, 1.0f } }
			};

			GeometryBufferCreateInfo verticesInfo
			{
				squareVertices,
				sizeof(SquareVertex),
				4,
				m_indices
			};

			m_triangleMesh = std::make_unique<StaticMesh>(*m_context, "TriangleMesh", verticesInfo);
			m_context->upload(m_triangleMesh->getVertexBuffer(), m_triangleMesh->getVertexBufferCpuData());
			m_context->upload(m_triangleMesh->getIndexBuffer(), m_triangleMesh->getIndexBufferCpuData().data());
		}

		m_context->init();
	}

	void HelloTriangleRenderer::onResizeWindow(const UINT width, const UINT height)
	{
		m_displayWidth = width;
		m_displayHeight = height;
		m_context->onResizeWindow(width, height);
	}

	void HelloTriangleRenderer::update(const float dt)
	{
		m_constantBufferCpuData.offset.x += 0.001f * dt;

		m_constantBufferGpuData[m_context->getFrameIndex()]->map(0, 0, reinterpret_cast<void**>(&m_constantBufferCpuVisibleGpuPointers[m_context->getFrameIndex()]));
		memcpy(m_constantBufferCpuVisibleGpuPointers[m_context->getFrameIndex()], &m_constantBufferCpuData,
		       sizeof(m_constantBufferCpuData));
		m_constantBufferGpuData[m_context->getFrameIndex()]->unmap();
	}

	void HelloTriangleRenderer::render() const {
		m_context->preRecording();
		m_context->clearBackBufferColor();

		m_context->bindRootSignature(*m_helloTriangleRootSignature);
		m_context->bindPipelineState(*m_helloTrianglePipeline);

		const std::vector heaps { *m_inputDescriptorHeap };
		m_context->setDescriptorHeaps(heaps);

		m_context->setGraphicsRootDescriptorTable(0, *m_inputDescriptorHeap, m_context->getFrameIndex());
		m_context->setGraphicsRootDescriptorTable(1, *m_inputDescriptorHeap, 2);

		m_context->setTopologyTypeTriangleList(Snd::Dx12::PrimitiveTopology::TriangleList);
		m_context->bindVertexBufferView(*m_triangleMesh->getVertexBufferView());
		m_context->bindIndexBufferView(*m_triangleMesh->getIndexBufferView());
		m_context->drawIndexedInstanced(
			m_indices.size(),
			1,
			0,
			0,
			0);

		m_context->postRecording();

		m_context->flush();

		m_context->moveToNextFrame();
	}
}
