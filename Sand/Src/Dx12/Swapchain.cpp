#include "Snd/Pch.h"
#include "Snd/Dx12/Swapchain.h"

#include "Snd/Dx12/CommandQueue.h"
#include "Snd/Dx12/GraphicsCommandList.h"
#include "Snd/Dx12/DescriptorHeap.h"
#include "Snd/Dx12/Fence.h"
#include "Snd/Dx12/Texture.h"
#include "Snd/Dx12/Utils/DxHelpers.h"


using Microsoft::WRL::ComPtr;

namespace Snd::Dx12
{
	Swapchain::Swapchain(
	    const std::shared_ptr<Device>& device, const std::shared_ptr<CommandQueue> &commandQueue,
	    const HWND& hWnd,  const UINT width, const UINT height)
        :  m_device { device }, m_commandQueue{commandQueue}
	{
        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = Swapchain::c_sFrameCount;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGIFactory4> dxgiFactory;
        ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_commandQueue.lock()->getDxCommandQueue().Get(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
        ));

        // This sample does not support fullscreen transitions.
        ThrowIfFailed(dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
        ThrowIfFailed(swapChain.As(&m_swapchain));
        updateFrameIndex();

        m_rtvHandle = std::make_unique<DescriptorHeap>("Swapchain rtv heap", *device, DescriptorHeapType::Rtv, c_sFrameCount);

        m_renderTargets.resize(c_sFrameCount);
        createRenderTargets();

        m_fence = std::make_unique<Fence>("Swapchain frame fence", *device);
	}

    Swapchain::~Swapchain() = default;

    void Swapchain::resize(const UINT width, const UINT height)
    {
        waitGpu();
        setFenceValuesToCurrentFrame();
        resetRenderTargets();
        resizeBuffers(width, height);
        createRenderTargets();
        updateFrameIndex();
    }

    void Swapchain::moveToNextFrame()
    {
        checkExpired();

        present();

        m_fence->scheduleSignalCommand(*m_commandQueue.lock(), m_fenceValues[m_frameIndex]);
        const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
        updateFrameIndex();
        if(m_fence->checkForWait(m_fenceValues[m_frameIndex]))
        {
            m_fence->waitForGpu(m_fenceValues[m_frameIndex]);
        }
        m_fenceValues[m_frameIndex] = currentFenceValue + 1;
    }

    void Swapchain::waitGpu()
    {
        checkExpired();

        m_fence->scheduleSignalCommand(*m_commandQueue.lock(), m_fenceValues[m_frameIndex]);

        m_fence->waitForGpu(m_fenceValues[m_frameIndex]);

        m_fenceValues[m_frameIndex]++;

        updateFrameIndex();
    }

    bool Swapchain::isValid() const
    {
        return m_swapchain != nullptr;
    }

    void Swapchain::swapCurrentBufferState(const GraphicsCommandList& commandList, const ResourceState afterState) const
    {
        m_renderTargets[m_frameIndex]->transition(commandList, afterState);
    }

    void Swapchain::setCurrentBufferAsRenderTarget(const GraphicsCommandList& commandList) const
    {
        const DescriptorCpuHandle rtvHandle = m_rtvHandle->hCpu(m_frameIndex);
        commandList.setRenderTarget(rtvHandle);
    }

    void Swapchain::clearColor(const GraphicsCommandList& commandList, const float clearColor[]) const
    {
        const DescriptorCpuHandle rtvHandle = m_rtvHandle->hCpu(m_frameIndex);
        commandList.clearRenderTargetView(rtvHandle, clearColor);
    }

    Microsoft::WRL::ComPtr<IDXGISwapChain3> Swapchain::getDxSwapchain() const
    {
        return m_swapchain;
    }

    void Swapchain::updateFrameIndex()
    {
        m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();
    }

    void Swapchain::createRenderTargets()
    {
        checkExpired();

        for (UINT i = 0; i < c_sFrameCount; i++)
        {
            m_renderTargets[i] = std::make_unique<SwapchainRenderTarget>(*m_device.lock(), *m_rtvHandle, *this, i);
        }
    }

    void Swapchain::resetRenderTargets()
    {
        for (UINT i = 0; i < c_sFrameCount; i++)
        {
            m_renderTargets[i].reset();
        }
    }

    void Swapchain::setFenceValuesToCurrentFrame()
    {
        for (unsigned long long& fenceValue : m_fenceValues)
        {
            fenceValue = m_fenceValues[m_frameIndex];
        }
    }

    void Swapchain::resizeBuffers(const UINT width, const UINT height) const
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        ThrowIfFailed(m_swapchain->GetDesc1(&desc));
        ThrowIfFailed(m_swapchain->ResizeBuffers(c_sFrameCount, width, height, desc.Format, desc.Flags));
    }

    void Swapchain::present() const
    {
        ThrowIfFailed(m_swapchain->Present(1, 0));
    }

    void Swapchain::checkExpired() const
    {
        NOMAD_ASSERT(Nmd::AssertType::Assert, !m_device.expired() && !m_commandQueue.expired(), "trying to call {} from a swapchain with expired device or command queue "
                                                                       "device and command queues life time should be longer then swapchain", __func__);
    }
}
