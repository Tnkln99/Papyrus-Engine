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

#include <cassert>

namespace Snd::Dx12
{
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
        m_isOnRenderLoop = true;
        m_graphicsCommandAllocators[m_swapchain->getFrameIndex()]->reset();
        m_graphicsCommandList->reset(*m_graphicsCommandAllocators[m_swapchain->getFrameIndex()]);

        m_swapchain->swapCurrentBufferState(*m_graphicsCommandList, ResourceState::RenderTarget);
        m_swapchain->setCurrentBufferAsRenderTarget(*m_graphicsCommandList);

        m_graphicsCommandList->setViewports(*m_viewPort);
        m_graphicsCommandList->setScissorRects(*m_scissorRect);
    }

    void Context::upload(const std::shared_ptr<Buffer>& bufferToUpload, void* data) const
    {
        m_uploadContext->registerBufferForUpload(bufferToUpload, data);
    }

    void Context::upload(const std::shared_ptr<Texture2D>& textureToUpload, void *data) const
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

    void Context::setTopologyTypeTriangleList(const PrimitiveTopology topology) const
    {
        m_graphicsCommandList->setTopologyTypeTriangleList(topology);
    }

    void Context::bindVertexBufferView(const VertexBufferView& view) const
    {
        m_graphicsCommandList->bindVertexBufferView(view);
    }

    void Context::drawInstanced(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation) const
    {
        m_graphicsCommandList->drawInstanced(
            vertexCountPerInstance, 
            instanceCount, 
            startVertexLocation, 
            startInstanceLocation);
    }

    void Context::setDescriptorHeaps(const std::vector<DescriptorHeap> &descriptorHeaps) const
    {
        m_graphicsCommandList->setDescriptorHeaps(descriptorHeaps);
    }

    void Context::setGraphicsRootDescriptorTable(const UINT rootParameterIndex, const DescriptorHeap& descriptorHeap, const UINT descriptorIndex) const
    {
        m_graphicsCommandList->setGraphicsRootDescriptorTable(rootParameterIndex, descriptorHeap, descriptorIndex);
    }

    void Context::transition(const std::shared_ptr<Resource>& resource, const ResourceState afterState, const UINT subresource, const ResourceBarrierFlag::Type flags,
        const ResourceBarrierType type) const
    {
        resource->transition(*m_graphicsCommandList, afterState, subresource, flags, type);
    }

    void Context::clearBackBufferColor() const
    {
	    constexpr float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        m_swapchain->clearColor(*m_graphicsCommandList, clearColor);
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
