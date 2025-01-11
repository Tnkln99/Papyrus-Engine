#include "Crv/Renderers/StaticModelRenderer.h"

#include <ranges>

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
        void render() const;

        void registerModel(StaticModelCreateInfo modelInfo);
        void unregisterModel(const std::string &name);

        // returns instance id who has been created
        uint32_t addModelInstance(const std::string &modelName, const DirectX::XMFLOAT4X4 &modelMatrix);
        void removeModelInstance(const std::string &m_modelName, uint32_t instanceId);

        void setTransformStaticModelInstance(const std::string &m_modelName, uint32_t instanceId, const DirectX::XMFLOAT4X4 &modelMatrix);

    private:
        struct ModelInstanceData
        {
            DirectX::XMFLOAT4X4 m_transforms;
        };

        struct CameraCpuData
        {
            DirectX::XMFLOAT4X4 m_viewMatrix;
            DirectX::XMFLOAT4X4 m_projectionMatrix;
        };

        struct RegisteredStaticModel
        {
            StaticModel m_staticModel;
            uint32_t m_instanceCount = 0;
        };

    private:
        const static uint32_t s_cMaxInstanceCount;

        CameraCpuData m_constantBufferCpuData{};

        std::unordered_map<std::string, RegisteredStaticModel> m_models;
        std::unordered_map<std::string, std::vector<ModelInstanceData>> m_modelToInstances;

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

        std::vector<std::shared_ptr<Snd::Dx12::Buffer>> m_instancesConstantBufferGpuData{}; // multiple for multiple buffering
        std::vector<UINT8*> m_instancesConstantBufferCpuVisibleGpuPointers; // multiple for multiple buffering
    };

    StaticModelRenderer::StaticModelRenderer(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight)
    : m_impl(std::make_unique<Impl>(hwnd, displayWidth, displayHeight)) {}

    StaticModelRenderer::Impl::Impl(const HWND& hwnd, const uint32_t displayWidth, const uint32_t displayHeight)
    : m_hwnd(hwnd), m_displayWidth(displayWidth), m_displayHeight(displayHeight), m_context(m_hwnd, m_displayWidth, m_displayHeight) {}
    StaticModelRenderer::Impl::~Impl() = default;

    void StaticModelRenderer::Impl::init()
    {
        {
            m_descriptorHeap = std::make_unique<Snd::Dx12::DescriptorHeap>(
               "HelloTriangle input Descriptor",
                  *m_context.getDevice(),
                  Snd::Dx12::DescriptorHeapType::CbvSrvUav,
                  Snd::Dx12::Context::getFrameCount(), // multiple constant buffers for multiple buffering
                  true);


            m_cameraConstantBufferCpuVisibleGpuPointers.resize(Snd::Dx12::Context::getFrameCount());
            m_cameraConstantBufferGpuData.resize(Snd::Dx12::Context::getFrameCount());

            for (int i = 0; i < Snd::Dx12::Context::getFrameCount(); ++i)
            {
                m_cameraConstantBufferGpuData[i] = std::make_shared<Snd::Dx12::Buffer>(
                                    "ConstantBuffer",
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
        }

        {
            const std::vector inputTableParameterCbv
            {
                Snd::Dx12::DescriptorTableParameter{Snd::Dx12::DescriptorTableParameterType::Cbv, 1, 0},
            };

            std::vector rootParameters
            {
                Snd::Dx12::RootParameter{Snd::Dx12::RootParameterType::Table, Snd::Dx12::ShaderVisibility::Vertex, inputTableParameterCbv},
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
    }

    void StaticModelRenderer::Impl::onResizeWindow(const uint32_t width, const uint32_t height) const
    {
        m_context.onResizeWindow(width, height);
    }

    void StaticModelRenderer::Impl::updateCamera(const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix)
    {
        m_constantBufferCpuData.m_projectionMatrix = projectionMatrix;
        m_constantBufferCpuData.m_viewMatrix = viewMatrix;

        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->map(0, 0, reinterpret_cast<void**>(&m_cameraConstantBufferCpuVisibleGpuPointers[m_context.getFrameIndex()]));
        memcpy(m_cameraConstantBufferCpuVisibleGpuPointers[m_context.getFrameIndex()], &m_constantBufferCpuData,
               sizeof(m_constantBufferCpuData));
        m_cameraConstantBufferGpuData[m_context.getFrameIndex()]->unmap();
    }

    void StaticModelRenderer::Impl::update(float dt)
    {
    }

    void StaticModelRenderer::Impl::render() const
    {
        for (const auto &model: m_models | std::views::values)
        {
            for (const auto& mesh : model.m_staticModel.m_meshes)
            {
                m_context.drawIndexedInstanced(
                    mesh.getIndexBufferCpuData().size(),
                    model.m_instanceCount,
                    0,
                    0,
                    0);
            }
        }
    }

    void StaticModelRenderer::Impl::registerModel(StaticModelCreateInfo modelInfo)
    {
        if (const auto it = m_modelToInstances.find(modelInfo.m_name); it != m_modelToInstances.end())
        {
            NOMAD_LOG(Nmd::LogLevel::Warning, "Trying to register a model already registered {}", modelInfo.m_name);
            return;
        }

        RegisteredStaticModel registeredModel;

        registeredModel.m_staticModel.m_name = modelInfo.m_name;

        for (const auto& meshInfo : modelInfo.m_meshes)
        {
            StaticMesh mesh{m_context, meshInfo.m_name, meshInfo.m_verticesInfo};
            registeredModel.m_staticModel.m_meshes.emplace_back(mesh);
            m_context.upload(mesh.getVertexBuffer(), mesh.getVertexBufferCpuData());
        }

        m_models[modelInfo.m_name] = registeredModel;

    }

    void StaticModelRenderer::Impl::unregisterModel(const std::string &name)
    {
        const auto& it = m_models.find(name);
        if (it == m_models.end())
        {
            NOMAD_LOG(Nmd::LogLevel::Warning, "Trying to unregister a model that is not registered");
            return;
        }

        m_models.erase(it);
        it->second.m_instanceCount--;;
    }

    uint32_t StaticModelRenderer::Impl::addModelInstance(const std::string& modelName, const DirectX::XMFLOAT4X4& modelMatrix)
    {
        const auto it = m_modelToInstances.find(modelName);

        NOMAD_ASSERT(Nmd::AssertType::Assert, it != m_modelToInstances.end(), "Model name not found in m_modelToInstances.");

        auto& transforms = it->second;
        transforms.emplace_back(modelMatrix);

        m_models[modelName].m_instanceCount++;

        NOMAD_ASSERT(Nmd::AssertType::Assert, m_models[modelName].m_instanceCount == transforms.size(), "Something went wrong with instance counting");

        return static_cast<uint32_t>(transforms.size() - 1);
    }

    void StaticModelRenderer::Impl::removeModelInstance(const std::string& m_modelName, const uint32_t instanceId)
    {
        const auto it = m_modelToInstances.find(m_modelName);
        NOMAD_ASSERT(Nmd::AssertType::Assert, it != m_modelToInstances.end(), "Invalid instanceId: Out of range for the model's transforms.");

        auto& transforms = it->second;
        NOMAD_ASSERT(Nmd::AssertType::Assert, instanceId < transforms.size(), "Invalid instanceId: Out of range for the model's transforms.");

        if (instanceId != transforms.size() - 1)
        {
            std::swap(transforms[instanceId], transforms.back());
        }

        transforms.pop_back();
    }

    void StaticModelRenderer::Impl::setTransformStaticModelInstance(const std::string &m_modelName, const uint32_t instanceId,
        const DirectX::XMFLOAT4X4 &modelMatrix)
    {
        const ModelInstanceData modelInstanceData{modelMatrix};
        m_modelToInstances[m_modelName][instanceId] = modelInstanceData;
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

    void StaticModelRenderer::render() const
    {
        m_impl->render();
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::registerModel(const StaticModelCreateInfo &modelInfo)
    {
        m_impl->registerModel(modelInfo);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::unregisterModel(const std::string &name)
    {
        m_impl->unregisterModel(name);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    uint32_t StaticModelRenderer::addModelInstance(const std::string &m_modelName, DirectX::XMFLOAT4X4 modelMatrix)
    {
        return m_impl->addModelInstance(m_modelName, modelMatrix);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::removeModelInstance(const std::string &m_modelName, uint32_t instanceId)
    {
        return m_impl->removeModelInstance(m_modelName, instanceId);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void StaticModelRenderer::setTransformStaticModelInstance(const std::string& m_modelName, const uint32_t instanceId,
        const DirectX::XMFLOAT4X4 &modelMatrix)
    {
        return m_impl->setTransformStaticModelInstance(m_modelName, instanceId, modelMatrix);
    }
}
