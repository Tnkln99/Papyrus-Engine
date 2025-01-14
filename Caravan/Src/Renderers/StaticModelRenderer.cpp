#include "Crv/Renderers/StaticModelRenderer.h"

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

#include "VxStaticModel.h"
#include "PxStaticModel.h"

namespace Crv
{
    class StaticModelRenderer::Impl
    {
    public:
        Impl(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight);
        ~Impl();

        void init();
        void onResizeWindow(uint32_t width, uint32_t height) const;

        void updateCamera(const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix);
        void update(float dt);
        void render();

        void changeModel(const StaticModelCreateInfo &modelInfo);

        // returns instance id who has been created
        uint32_t addModelInstance(const DirectX::XMFLOAT4X4 &modelMatrix);
        void removeModelInstance(uint32_t instanceId);

        void setTransformStaticModelInstance(uint32_t instanceId, const DirectX::XMFLOAT4X4 &modelMatrix);

    private:
        struct ModelInstanceData
        {
            DirectX::XMFLOAT4X4 m_transforms;
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
        static constexpr uint32_t s_cMaxInstanceCount = 1000;

        CameraCpuData m_constantBufferCpuData{};

        RegisteredStaticModel m_registeredStaticModel{};
        std::vector<ModelInstanceData> m_instanceDatas;

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

        std::vector<std::shared_ptr<Snd::Dx12::Buffer>> m_instancesBufferGpuData{}; // multiple for multiple buffering
        std::vector<UINT8*> m_instancesBufferCpuVisibleGpuPointers; // multiple for multiple buffering
    };

    StaticModelRenderer::StaticModelRenderer(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight)
    : m_impl(std::make_unique<Impl>(hwnd, displayWidth, displayHeight)) {}

    StaticModelRenderer::Impl::Impl(const HWND& hwnd, const uint32_t displayWidth, const uint32_t displayHeight)
    : m_hwnd(hwnd), m_displayWidth(displayWidth), m_displayHeight(displayHeight), m_context(m_hwnd, m_displayWidth, m_displayHeight) {}
    StaticModelRenderer::Impl::~Impl() = default;

    void StaticModelRenderer::Impl::init()
    {
        // resource creation
        {
            m_descriptorHeap = std::make_unique<Snd::Dx12::DescriptorHeap>(
               "StaticModelRenderer Input Descriptor",
                  *m_context.getDevice(),
                  Snd::Dx12::DescriptorHeapType::CbvSrvUav,
                  Snd::Dx12::Context::getFrameCount() * 2, // multiple constant buffers for multiple buffering
                  true);


            m_cameraConstantBufferCpuVisibleGpuPointers.resize(Snd::Dx12::Context::getFrameCount());
            m_cameraConstantBufferGpuData.resize(Snd::Dx12::Context::getFrameCount());
            m_instancesBufferGpuData.resize(Snd::Dx12::Context::getFrameCount());
            m_instancesBufferCpuVisibleGpuPointers.resize(Snd::Dx12::Context::getFrameCount());

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
                    m_instancesBufferGpuData[i] = std::make_shared<Snd::Dx12::Buffer>(
                                    "InstancesData Buffer",
                                    *m_context.getDevice(),
                                    Snd::Dx12::ResourceHeapType::Upload,
                                    Snd::Dx12::ResourceFlag::None,
                                    Snd::Dx12::ResourceState::Common,
                                    sizeof(ModelInstanceData), s_cMaxInstanceCount);

                    m_descriptorHeap->newShaderResourceView( *m_context.getDevice(), *m_instancesBufferGpuData[i], i + Snd::Dx12::Context::getFrameCount());
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
                Snd::Dx12::RootParameter{Snd::Dx12::RootParameterType::Table, Snd::Dx12::ShaderVisibility::Vertex, inputTableParameterSrv}
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
            const std::vector inputAssemblyParameters
            {
                Snd::InputParameter{ "POSITION", Snd::InputParameterType::Vector4, 0, 0, Snd::InputRate::PerVertex },
                Snd::InputParameter{ "NORMAL", Snd::InputParameterType::Vector4, 16, 0, Snd::InputRate::PerVertex },
                Snd::InputParameter{ "TEXCOORD", Snd::InputParameterType::Vector2, 32, 0, Snd::InputRate::PerVertex },
            };

            constexpr Snd::ShaderCode vertexCode{ VxStaticModel, sizeof(VxStaticModel) };
            constexpr Snd::ShaderCode pixelCode{ PxStaticModel, sizeof(PxStaticModel) };

            Snd::Dx12::ClassicGraphicsPipelineDescriptor graphicsPipelineDesc
            {
                *m_rootSignature,
                vertexCode,
                pixelCode,
                inputAssemblyParameters
            };

            m_pipelineState = std::make_unique<Snd::Dx12::PipelineState>("StaticModel PipelineState", *m_context.getDevice(), graphicsPipelineDesc);
        }

        m_context.init();
    }

    void StaticModelRenderer::Impl::onResizeWindow(const uint32_t width, const uint32_t height) const
    {
        m_context.onResizeWindow(width, height);
    }

    void StaticModelRenderer::Impl::updateCamera(const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix)
    {
        m_constantBufferCpuData.m_projectionMatrix = projectionMatrix;
        m_constantBufferCpuData.m_viewMatrix = viewMatrix;

        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->map(0, 0,
            reinterpret_cast<void**>(&m_cameraConstantBufferCpuVisibleGpuPointers[m_context.getFrameIndex()]));
        memcpy(m_cameraConstantBufferCpuVisibleGpuPointers[m_context.getFrameIndex()], &m_constantBufferCpuData,
               sizeof(m_constantBufferCpuData));
        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->unmap();
    }

    void StaticModelRenderer::Impl::update(float dt)
    {
        m_instancesBufferGpuData[m_context.getFrameIndex()]->map(0, 0,
            reinterpret_cast<void**>(&m_instancesBufferCpuVisibleGpuPointers[m_context.getFrameIndex()]));
        memcpy(m_instancesBufferCpuVisibleGpuPointers[m_context.getFrameIndex()], m_instanceDatas.data(),
               sizeof(ModelInstanceData) * m_instanceDatas.size());
        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->unmap();
    }

    void StaticModelRenderer::Impl::render()
    {
        m_context.preRecording();
        m_context.clearBackBufferColor();

        m_context.bindRootSignature(*m_rootSignature);
        m_context.bindPipelineState(*m_pipelineState);

        const std::vector heaps { *m_descriptorHeap };
        m_context.setDescriptorHeaps(heaps);

        m_context.setGraphicsRootDescriptorTable(0, *m_descriptorHeap, m_context.getFrameIndex());
        m_context.setGraphicsRootDescriptorTable(0, *m_descriptorHeap, m_context.getFrameIndex()+2);

        m_context.setTopologyTypeTriangleList(Snd::Dx12::PrimitiveTopology::TriangleList);

        for(const auto& mesh : m_registeredStaticModel.m_staticModel.m_meshes)
        {
            m_context.bindVertexBufferView(*mesh.getVertexBufferView());
            m_context.bindIndexBufferView(*mesh.getIndexBufferView());

            m_context.drawIndexedInstanced(
                mesh.getIndexBufferCpuData().size(),
                m_registeredStaticModel.m_instanceCount,
                0,
                0,
                0);
        }

        m_context.postRecording();
        m_context.flush();
        m_context.moveToNextFrame();
    }

    void StaticModelRenderer::Impl::changeModel(const StaticModelCreateInfo &modelInfo)
    {
        m_registeredStaticModel.m_staticModel.m_meshes.clear();
        m_registeredStaticModel.m_staticModel.m_meshes.reserve(modelInfo.m_meshes.size());

        for (const auto& meshInfo : modelInfo.m_meshes)
        {
            m_registeredStaticModel.m_staticModel.m_meshes.emplace_back( m_context, meshInfo.m_name, meshInfo.m_verticesInfo );
            const StaticMesh& meshToUpload = m_registeredStaticModel.m_staticModel.m_meshes.back();
            m_context.upload(meshToUpload.getVertexBuffer(), meshToUpload.getVertexBufferCpuData());
            m_context.upload(meshToUpload.getIndexBuffer(), meshToUpload.getIndexBufferCpuData().data());
        }
    }

    uint32_t StaticModelRenderer::Impl::addModelInstance(const DirectX::XMFLOAT4X4 &modelMatrix)
    {
        m_instanceDatas.emplace_back(modelMatrix);
        return m_instanceDatas.size() - 1;
    }

    void StaticModelRenderer::Impl::removeModelInstance(const uint32_t instanceId)
    {
        if (m_instanceDatas.size() <= instanceId)
        {
            NOMAD_LOG(Nmd::LogLevel::Warning, "Trying to remove a instance that cant exists");
            return;
        }
        auto& it = m_instanceDatas.at(instanceId);

        std::swap(it, m_instanceDatas.back());
        m_instanceDatas.pop_back();
    }

    void StaticModelRenderer::Impl::setTransformStaticModelInstance(const uint32_t instanceId,
                                                                    const DirectX::XMFLOAT4X4 &modelMatrix)
    {
        ModelInstanceData &instanceData = m_instanceDatas[instanceId];
        instanceData.m_transforms = modelMatrix;
    }


    StaticModelRenderer::~StaticModelRenderer() = default;

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::init()
    {
        m_impl->init();
    }

    void StaticModelRenderer::onResizeWindow(const uint32_t width, const uint32_t height) const
    {
        m_impl->onResizeWindow(width, height);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::updateCamera(const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projectionMatrix)
    {
        m_impl->updateCamera(viewMatrix, projectionMatrix);
    }

    void StaticModelRenderer::update(float dt)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::render()
    {
        m_impl->render();
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::changeModel(const StaticModelCreateInfo &modelInfo)
    {
        m_impl->changeModel(modelInfo);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    uint32_t StaticModelRenderer::addModelInstance(const DirectX::XMFLOAT4X4 &modelMatrix)
    {
        return m_impl->addModelInstance(modelMatrix);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::removeModelInstance(const uint32_t instanceId)
    {
        return m_impl->removeModelInstance(instanceId);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::setTransformStaticModelInstance(const uint32_t instanceId,
        const DirectX::XMFLOAT4X4 &modelMatrix)
    {
        return m_impl->setTransformStaticModelInstance(instanceId, modelMatrix);
    }
}
