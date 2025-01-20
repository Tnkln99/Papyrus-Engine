#pragma once

#include <memory>
#include <vector>

namespace Snd::Dx12
{
	enum class ResourceState;

	class Device;
	class CommandQueue;
	class SwapchainRenderTarget;
	class DescriptorHeap;
	class Fence;
	class GraphicsCommandList;
	class Texture2D;

	class Swapchain
	{
	public:
		static constexpr UINT c_sFrameCount{ 2 };

	public:
		Swapchain(
			const std::shared_ptr<Device>& device, const std::shared_ptr<CommandQueue> &commandQueue,
			const HWND& hWnd,  UINT width, UINT height);
		~Swapchain();

		[[nodiscard]] UINT getFrameIndex() const { return m_frameIndex; }

		void resize(UINT width, UINT height);

		void moveToNextFrame();
		// will wait for gpu until current frame is rendered
		void waitGpu();

		void swapCurrentBufferState(const GraphicsCommandList& commandList, ResourceState afterState) const;
		void setCurrentBufferAsRenderTarget(const GraphicsCommandList& commandList) const;

		void clear(const GraphicsCommandList& commandList, const float clearColor[]) const;

		[[nodiscard]] Microsoft::WRL::ComPtr<IDXGISwapChain3> getDxSwapchain() const;
		[[nodiscard]] bool isValid() const;

	private:
		// set the frame index
		void updateFrameIndex();

		void createRenderTargets();
		void createDepthResources();

		void reset();
		void setFenceValuesToCurrentFrame();
		void resizeBuffers(UINT width, UINT height) const;

		void present() const;

	private:
		void checkExpired() const;

	private:

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapchain;

		std::weak_ptr<Device> m_device;
		std::weak_ptr<CommandQueue> m_commandQueue;

		std::unique_ptr<DescriptorHeap> m_rtvHandle;
		std::vector<std::unique_ptr<SwapchainRenderTarget>> m_renderTargets;

		std::unique_ptr<DescriptorHeap> m_dsvHandle;
		std::vector<std::unique_ptr<Texture2D>> m_depthStencils;
		
		std::unique_ptr<Fence> m_fence;
		UINT64 m_fenceValues[c_sFrameCount]{};

		UINT m_frameIndex{};
	};
}
