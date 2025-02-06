#include "Snd/Pch.h"
#include "Snd/Dx12/Context.h"

#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/Swapchain.h"
#include "Snd/Dx12/CommandQueue.h"
#include "Snd/Dx12/GraphicsCommandList.h"
#include "Snd/Dx12/CommandAllocator.h"
#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/DescriptorHeap.h"
#include "Snd/Dx12/UploadContext.h"
#include "Snd/Dx12/Viewport.h"
#include "Snd/Dx12/ScissorRect.h"
#include "Snd/Dx12/Utils/Common.h"

#include "Nmd/Asserter.h"

#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

#include <cassert>


namespace Snd::Dx12
{
    std::unique_ptr<DescriptorHeap> Context::s_imguiSrvDescriptorHeap = nullptr;
    std::deque<UINT> Context::s_imguiSrvDescriptorFreeIndices = std::deque<UINT>();

    Context::Context(const HWND& hWnd, const UINT displayWidth, const UINT displayHeight)
    {
        m_device = std::make_shared<Device>("GraphicsContext Device");
        
        m_graphicsCommandQueue = std::make_shared<CommandQueue>("GraphicsContext CommandQueue", *m_device, GraphicsCommandListType::Direct, CommandQueue::Priority::Normal, CommandQueue::Flag::None);
        
        m_swapchain = std::make_unique<Swapchain>(m_device, m_graphicsCommandQueue, hWnd,  displayWidth, displayHeight);

        m_graphicsCommandAllocators.reserve(Swapchain::c_sFrameCount);
        for(size_t i = 0; i < Swapchain::c_sFrameCount; i++)
        {
            std::string name = "GraphicsContext CommandAllocator" + std::to_string(i);
            m_graphicsCommandAllocators.push_back(std::make_unique<CommandAllocator>(name, *m_device, GraphicsCommandListType::Direct));
        }

        m_graphicsCommandList = std::make_unique<GraphicsCommandList>("GraphicsContext CommandList", *m_device, *m_graphicsCommandAllocators[0], GraphicsCommandListType::Direct);

		m_uploadContext = std::make_unique<UploadContext>(*m_device);

		m_viewPort = std::make_unique<Viewport>(0.0f, 0.0f, static_cast<float>(displayWidth), static_cast<float>(displayHeight));
		m_scissorRect = std::make_unique<ScissorRect>(0, 0, static_cast<LONG>(displayWidth), static_cast<LONG>(displayHeight));
    }

    Context::~Context()
    {
        m_swapchain->waitGpu();
    }

    void Context::onResizeWindow(const UINT width, const UINT height) const
    {
        m_swapchain->resize(width, height);

        m_viewPort->Width = static_cast<float>(width);
        m_viewPort->Height = static_cast<float>(height);

        m_scissorRect->right = static_cast<LONG>(width);
        m_scissorRect->bottom = static_cast<LONG>(height);
    }

    void Context::init() const
    {
        m_uploadContext->flush();

        m_graphicsCommandList->close();
        m_graphicsCommandQueue->executeSingleCommandList(*m_graphicsCommandList);

        m_swapchain->waitGpu();
    }

    void Context::preRecording()
    {
        m_bOnRenderLoop = true;
        m_graphicsCommandAllocators[m_swapchain->getFrameIndex()]->reset();
        m_graphicsCommandList->reset(*m_graphicsCommandAllocators[m_swapchain->getFrameIndex()]);

        m_swapchain->swapCurrentBufferState(*m_graphicsCommandList, ResourceState::RenderTarget);
        m_swapchain->setCurrentBufferAsRenderTarget(*m_graphicsCommandList);

        m_graphicsCommandList->setViewports(*m_viewPort);
        m_graphicsCommandList->setScissorRects(*m_scissorRect);
    }

    void Context::initImgui(const HWND &hWnd)
    {
        s_imguiSrvDescriptorHeap = std::make_unique<DescriptorHeap>
        (
            "Imgui Srv Descriptor Heap",
            *m_device,
            DescriptorHeapType::CbvSrvUav,
            s_cImguiDescriptorCount,
            true
        );

        for (int n = s_cImguiDescriptorCount; n > 0; n--)
        {
            s_imguiSrvDescriptorFreeIndices.push_back(n);
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        //GImGui->DebugLogFlags |= ImGuiDebugLogFlags_EventIO;

        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hWnd);

        ImGui_ImplDX12_InitInfo imguiInitInfo = {};
        imguiInitInfo.Device = m_device->getDxDevice().Get();
        imguiInitInfo.CommandQueue = m_graphicsCommandQueue->getDxCommandQueue().Get();
        imguiInitInfo.NumFramesInFlight = Swapchain::c_sFrameCount;
        imguiInitInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        imguiInitInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;

        imguiInitInfo.SrvDescriptorHeap = s_imguiSrvDescriptorHeap->getDxDescriptorHeap().Get();

        imguiInitInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
        {
            const UINT idx = s_imguiSrvDescriptorFreeIndices.back();
            s_imguiSrvDescriptorFreeIndices.pop_back();

            const DescriptorCpuHandle cpuHandle = s_imguiSrvDescriptorHeap->hCpu(idx);
            const DescriptorGpuHandle gpuHandle = s_imguiSrvDescriptorHeap->hGpu(idx);

            *out_cpu_handle = cpuHandle;
            *out_gpu_handle = gpuHandle;
        };

        imguiInitInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
        {
            const auto cpuStart = s_imguiSrvDescriptorHeap->getDxDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
            const auto gpuStart = s_imguiSrvDescriptorHeap->getDxDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

            const int cpuIdx = static_cast<int>((cpu_handle.ptr - cpuStart.ptr) / s_imguiSrvDescriptorHeap->getIncrementSize());
            const int gpuIdx = static_cast<int>((gpu_handle.ptr - gpuStart.ptr) / s_imguiSrvDescriptorHeap->getIncrementSize());

            NOMAD_ASSERT(Nmd::AssertType::Assert, cpuIdx == gpuIdx, "Something went wrong while imgui was freeing space");

            s_imguiSrvDescriptorFreeIndices.push_back(cpuIdx);
        };

        ImGui_ImplDX12_Init(&imguiInitInfo);

        m_bImguiInitiated = true;
    }

    void Context::newFrameImgui() const
    {
        NOMAD_ASSERT(Nmd::AssertType::Assert, m_bImguiInitiated == true, "Imgui should be initiated");
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void Context::renderImgui() const
    {
        NOMAD_ASSERT(Nmd::AssertType::Assert, m_bImguiInitiated == true, "Imgui should be initiated");
        ImGui::Render();

        const std::vector heaps { *s_imguiSrvDescriptorHeap };
        m_graphicsCommandList->setDescriptorHeaps(heaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_graphicsCommandList->getDxGraphicsCommandList().Get());
    }

    void Context::cleanUpImgui() const
    {
        NOMAD_ASSERT(Nmd::AssertType::Assert, m_bImguiInitiated == true, "Imgui should be initiated");
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void Context::upload(const std::shared_ptr<Buffer>& bufferToUpload, const void* data) const
    {
        m_uploadContext->registerBufferForUpload(bufferToUpload, data);
    }

    void Context::upload(const std::shared_ptr<Texture2D>& textureToUpload, const void *data) const
    {
        m_uploadContext->registerTexture2DForUpload(textureToUpload, data);
    }

    void Context::bindRootSignature(const RootSignature& rootSignature) const
    {
        m_graphicsCommandList->bindRootSignature(rootSignature);
    }

    void Context::bindPipelineState(const PipelineState& pipelineState) const
    {
        m_graphicsCommandList->bindPipelineState(pipelineState);
    }

    void Context::setTopology(const PrimitiveTopology topology) const
    {
        m_graphicsCommandList->setTopologyTypeTriangleList(topology);
    }

    void Context::bindVertexBufferView(const VertexBufferView& view) const
    {
        m_graphicsCommandList->bindVertexBufferView(view);
    }

    void Context::bindIndexBufferView(const IndexBufferView &view) const
    {
        m_graphicsCommandList->bindIndexBufferView(view);
    }

    void Context::drawInstanced(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation) const
    {
        m_graphicsCommandList->drawInstanced(
            vertexCountPerInstance, 
            instanceCount, 
            startVertexLocation, 
            startInstanceLocation);
    }

    void Context::drawIndexedInstanced(const UINT indexCountPerInstance, const UINT instanceCount, const UINT startIndexLocation,
        const INT baseVertexLocation, const UINT startInstanceLocation) const
    {
        m_graphicsCommandList->drawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void Context::setDescriptorHeaps(const std::vector<DescriptorHeap> &descriptorHeaps) const
    {
        m_graphicsCommandList->setDescriptorHeaps(descriptorHeaps);
    }

    void Context::setGraphicsRootDescriptorTable(const UINT rootParameterIndex, const DescriptorHeap& descriptorHeap, const UINT descriptorIndex) const
    {
        m_graphicsCommandList->setGraphicsRootDescriptorTable(rootParameterIndex, descriptorHeap, descriptorIndex);
    }

    void Context::transition(const std::shared_ptr<Resource>& resource,
        const ResourceState afterState,
        const UINT subresource,
        const ResourceBarrierFlag::Type flags,
        const ResourceBarrierType type) const
    {
        resource->transition(*m_graphicsCommandList, afterState, subresource, flags, type);
    }

    void Context::clear() const
    {
	    constexpr float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        m_swapchain->clear(*m_graphicsCommandList, clearColor);
    }

    void Context::postRecording() const
    {
        m_swapchain->swapCurrentBufferState(*m_graphicsCommandList, ResourceState::Present);

        m_graphicsCommandList->close();
    }

    void Context::flush() const
    {
        m_uploadContext->flush();
        m_graphicsCommandQueue->executeSingleCommandList(*m_graphicsCommandList);
    }

    const std::shared_ptr<Device>& Context::getDevice() const
    {
        return m_device;
    }

    uint32_t Context::getFrameIndex() const
    {
        return m_swapchain->getFrameIndex();
    }

    UINT Context::getFrameCount()
    {
        return Swapchain::c_sFrameCount;
    }

    void Context::moveToNextFrame() const
    {
        m_swapchain->moveToNextFrame();
    }
}
