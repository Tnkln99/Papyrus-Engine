#include "Crv/Renderers/PointCloudRenderer.h"

#include "Crv/PrivateData/StaticModel.h"
#include "Crv/Data/StaticModelCreateInfo.h"

#include "Snd/Dx12/Context.h"
#include "Snd/Dx12/DescriptorHeap.h"
#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/RootSignature.h"
#include "Snd/Dx12/StaticSamplers.h"
#include "Snd/Dx12/Utils/Common.h"

#include "Snd/InputParameter.h"
#include "Snd/ShaderCode.h"
#include "Snd/Dx12/PipelineDescriptor.h"
#include "Snd/Dx12/PipelineState.h"

#include "Nmd/Logger.h"

#include <unordered_map>
#include <ranges>

#include "PointRenderer/VxPointRenderer.h"
#include "PointRenderer/PxPointRenderer.h"

#include "Crv/PrivateData/DataGenerators.h"

namespace Crv
{
    class PointCloudRenderer::Impl
    {
    public:
        Impl(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight);
        ~Impl();

        void init();
        void onResizeWindow(uint32_t width, uint32_t height) const;

        void updateCamera(const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix);
        void update(float dt);
        void render();

        // returns instance id who has been created
        uint32_t addPoint(const DirectX::XMFLOAT3 &position);
        void removePoint(uint32_t instanceId);

        void changePointLocation(uint32_t instanceId, const DirectX::XMFLOAT3 &position);

    private:
        struct alignas(16) PointData
        {
            DirectX::XMFLOAT3 m_position;
            float m_radius;
        };

        struct CameraCpuData
        {
            DirectX::XMFLOAT4X4 m_viewMatrix = DirectX::XMFLOAT4X4();
            DirectX::XMFLOAT4X4 m_projectionMatrix = DirectX::XMFLOAT4X4();
            float m_padding[32] = {}; // 128 bytes of padding (32 floats * 4 bytes per float)
        };
        static_assert((sizeof(CameraCpuData) % 256) == 0, "Constant Buffer size must be 256-byte aligned");


        struct RegisteredStaticModel
        {
            StaticModel m_staticModel;
            uint32_t m_instanceCount = 0;
        };

    private:
        static constexpr uint32_t s_cMaxPointCount = 10000000;

        CameraCpuData m_constantBufferCpuData{};

        // todo: change this to array with s_cMaxInstanceCount elements
        std::vector<PointData> m_pointDatas;

    private:
        const HWND& m_hwnd;
        UINT m_displayWidth = 0;
        UINT m_displayHeight = 0;

        Snd::Dx12::Context m_context;

        std::unique_ptr<Snd::Dx12::DescriptorHeap> m_descriptorHeap;

        std::unique_ptr<Snd::Dx12::PipelineState> m_pipelineState;
        std::unique_ptr<Snd::Dx12::RootSignature> m_rootSignature;

        std::vector<std::shared_ptr<Snd::Dx12::Buffer>> m_cameraConstantBufferGpuData{}; // multiple for multiple buffering
        std::vector<UINT8*> m_cameraConstantBufferCpuVisibleGpuPointers; // multiple for multiple buffering

        std::vector<std::shared_ptr<Snd::Dx12::Buffer>> m_pointInstancesBufferGpuData{}; // multiple for multiple buffering
        std::vector<UINT8*> m_pointInstancesBufferCpuVisibleGpuPointers; // multiple for multiple buffering
    };

    PointCloudRenderer::PointCloudRenderer(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight)
    : m_impl(std::make_unique<Impl>(hwnd, displayWidth, displayHeight)) {}

    PointCloudRenderer::Impl::Impl(const HWND& hwnd, const uint32_t displayWidth, const uint32_t displayHeight)
    : m_hwnd(hwnd), m_displayWidth(displayWidth), m_displayHeight(displayHeight), m_context(m_hwnd, m_displayWidth, m_displayHeight) {}
    PointCloudRenderer::Impl::~Impl() = default;

    void PointCloudRenderer::Impl::init()
    {
        // resource creation
        {
            m_descriptorHeap = std::make_unique<Snd::Dx12::DescriptorHeap>(
               "PointCloudRenderer Input Descriptor",
                  *m_context.getDevice(),
                  Snd::Dx12::DescriptorHeapType::CbvSrvUav,
                  Snd::Dx12::Context::getFrameCount() * 2, // multiple constant buffers for multiple buffering
                  true);


            m_cameraConstantBufferCpuVisibleGpuPointers.resize(Snd::Dx12::Context::getFrameCount());
            m_cameraConstantBufferGpuData.resize(Snd::Dx12::Context::getFrameCount());
            m_pointInstancesBufferGpuData.resize(Snd::Dx12::Context::getFrameCount());
            m_pointInstancesBufferCpuVisibleGpuPointers.resize(Snd::Dx12::Context::getFrameCount());

            for (int i = 0; i < Snd::Dx12::Context::getFrameCount(); ++i)
            {
                // creation of camera resource
                {
                    m_cameraConstantBufferGpuData[i] = std::make_shared<Snd::Dx12::Buffer>(
                                       "Camera Buffer",
                                       *m_context.getDevice(),
                                       Snd::Dx12::ResourceHeapType::Upload,
                                       Snd::Dx12::ResourceFlag::None,
                                       Snd::Dx12::ResourceState::Common,
                                       sizeof(CameraCpuData), 1);

                    m_cameraConstantBufferGpuData[i]->map(0, 0, reinterpret_cast<void**>(&m_cameraConstantBufferCpuVisibleGpuPointers[i]));
                    memcpy(m_cameraConstantBufferCpuVisibleGpuPointers[i], &m_constantBufferCpuData, sizeof(m_constantBufferCpuData));
                    m_cameraConstantBufferGpuData[i]->unmap();

                    m_descriptorHeap->newConstantBufferView(*m_context.getDevice(), *m_cameraConstantBufferGpuData[i], i);
                }

                // creation of instances data
                {
                    m_pointInstancesBufferGpuData[i] = std::make_shared<Snd::Dx12::Buffer>(
                                    "InstancesData Buffer",
                                    *m_context.getDevice(),
                                    Snd::Dx12::ResourceHeapType::Upload,
                                    Snd::Dx12::ResourceFlag::None,
                                    Snd::Dx12::ResourceState::Common,
                                    sizeof(PointData), s_cMaxPointCount);

                    m_descriptorHeap->newShaderResourceView( *m_context.getDevice(), *m_pointInstancesBufferGpuData[i], i + Snd::Dx12::Context::getFrameCount());
                }
            }
        }

        // root signature
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
                Snd::Dx12::RootParameter{Snd::Dx12::RootParameterType::Table, Snd::Dx12::ShaderVisibility::Vertex, inputTableParameterSrv},
            };

            Snd::Dx12::StaticSamplers samplers;

            m_rootSignature = std::make_unique<Snd::Dx12::RootSignature>(
                "StaticModel RootSignature",
                *m_context.getDevice(),
                samplers,
                rootParameters,
                Snd::Dx12::RootSignatureFlag::AllowInputAssembler);
        }

        // pipeline definition
        {
            const std::vector<Snd::InputParameter> inputAssemblyParameters{};

            constexpr Snd::ShaderCode vertexCode{ VxPointRenderer, sizeof(VxPointRenderer) };
            constexpr Snd::ShaderCode pixelCode{ PxPointRenderer, sizeof(PxPointRenderer) };

            Snd::Dx12::ClassicGraphicsPipelineDescriptor graphicsPipelineDesc
            {
                *m_rootSignature,
                vertexCode,
                pixelCode,
                inputAssemblyParameters
            };

            m_pipelineState = std::make_unique<Snd::Dx12::PipelineState>("StaticModel PipelineState", *m_context.getDevice(), graphicsPipelineDesc);
        }

        m_pointDatas.reserve(s_cMaxPointCount);

        m_context.init();
    }

    void PointCloudRenderer::Impl::onResizeWindow(const uint32_t width, const uint32_t height) const
    {
        m_context.onResizeWindow(width, height);
    }

    void PointCloudRenderer::Impl::updateCamera(const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix)
    {
        m_constantBufferCpuData.m_viewMatrix = viewMatrix;
        m_constantBufferCpuData.m_projectionMatrix = projectionMatrix;

        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->map(0, 0,
            reinterpret_cast<void**>(&m_cameraConstantBufferCpuVisibleGpuPointers[m_context.getFrameIndex()]));
        memcpy(m_cameraConstantBufferCpuVisibleGpuPointers[m_context.getFrameIndex()], &m_constantBufferCpuData,
               sizeof(CameraCpuData));
        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->unmap();
    }

    void PointCloudRenderer::Impl::update(float dt)
    {
        if (m_pointDatas.empty())
        {
            return;
        }

        m_pointInstancesBufferGpuData[m_context.getFrameIndex()]->map(0, 0,
            reinterpret_cast<void**>(&m_pointInstancesBufferCpuVisibleGpuPointers[m_context.getFrameIndex()]));
        memcpy(m_pointInstancesBufferCpuVisibleGpuPointers[m_context.getFrameIndex()], m_pointDatas.data(),
               sizeof(PointData) * m_pointDatas.size());
        m_pointInstancesBufferGpuData[m_context.getFrameIndex()]->unmap();
    }

    void PointCloudRenderer::Impl::render()
    {
        m_context.preRecording();
        m_context.clear();

        m_context.bindRootSignature(*m_rootSignature);
        m_context.bindPipelineState(*m_pipelineState);

        const std::vector heaps { *m_descriptorHeap };
        m_context.setDescriptorHeaps(heaps);

        m_context.setGraphicsRootDescriptorTable(0, *m_descriptorHeap, m_context.getFrameIndex());
        m_context.setGraphicsRootDescriptorTable(1, *m_descriptorHeap, m_context.getFrameIndex()+2);

        m_context.setTopology(Snd::Dx12::PrimitiveTopology::TriangleList);

        m_context.drawInstanced(
                6,
                m_pointDatas.size(),
                0,
                0);

        m_context.postRecording();
        m_context.flush();
        m_context.moveToNextFrame();
    }

    uint32_t PointCloudRenderer::Impl::addPoint(const DirectX::XMFLOAT3 &position)
    {
        if (m_pointDatas.size() >= s_cMaxPointCount)
        {
            NOMAD_LOG(Nmd::LogLevel::Warning, "Instance count exceeded");
            return -1;
        }
        m_pointDatas.emplace_back(position, 0.1f);
        return m_pointDatas.size() - 1;
    }

    void PointCloudRenderer::Impl::removePoint(const uint32_t instanceId)
    {
        if (m_pointDatas.size() <= instanceId)
        {
            NOMAD_LOG(Nmd::LogLevel::Warning, "Trying to remove a instance that cant exists");
            return;
        }
        auto& it = m_pointDatas.at(instanceId);

        std::swap(it, m_pointDatas.back());
        m_pointDatas.pop_back();
    }

    void PointCloudRenderer::Impl::changePointLocation(const uint32_t instanceId, const DirectX::XMFLOAT3 &position)
    {
        PointData &instanceData = m_pointDatas[instanceId];
        instanceData.m_position = position;
    }


    PointCloudRenderer::~PointCloudRenderer() = default;

    // ReSharper disable once CppMemberFunctionMayBeConst
    void PointCloudRenderer::init()
    {
        m_impl->init();
    }

    void PointCloudRenderer::onResizeWindow(const uint32_t width, const uint32_t height) const
    {
        m_impl->onResizeWindow(width, height);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void PointCloudRenderer::updateCamera(const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projectionMatrix)
    {
        m_impl->updateCamera(viewMatrix, projectionMatrix);
    }

    void PointCloudRenderer::update(float dt)
    {
        m_impl->update(dt);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void PointCloudRenderer::render()
    {
        m_impl->render();
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    uint32_t PointCloudRenderer::addPoint(const DirectX::XMFLOAT3 &position)
    {
        return m_impl->addPoint(position);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void PointCloudRenderer::removePoint(const uint32_t instanceId)
    {
        return m_impl->removePoint(instanceId);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void PointCloudRenderer::changePointLocation(const uint32_t instanceId,
        const DirectX::XMFLOAT3 &position)
    {
        return m_impl->changePointLocation(instanceId, position);
    }
}
