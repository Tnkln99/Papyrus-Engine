#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "Resource.h"

namespace Snd::Dx12
{
	class IndexBufferView;
	enum class ResourceState;
	enum class PrimitiveTopology;

	class Device;
	class Swapchain;
	class GraphicsCommandList;
	class CommandQueue;
	class CommandAllocator;
	class DescriptorHeap;
	class PipelineState;
	class RootSignature;
	class VertexBufferView;
	class Buffer;
	class Texture2D;

	struct UploadContext;
	struct Viewport;
	struct ScissorRect;


	class Context
	{
	public:
		Context(const HWND& hWnd, UINT displayWidth, UINT displayHeight);
		~Context();

		// this should be called before rendering starts, should be called once, just before rendering
		// will flush all the CommandList commands user listed before rendering starts
		void init() const;

		void onResizeWindow(UINT width, UINT height) const;

		void preRecording();

#pragma region Imgui communications
		// if renderer that uses Sans API wants to use Imgui, imgui should be initiated
		void initImgui(const HWND &hWnd);
		void newFrameImgui() const;
		void renderImgui() const;
		void cleanUpImgui() const;
#pragma endregion

#pragma region commammand list communications
		void upload(const std::shared_ptr<Buffer>& bufferToUpload, const void* data) const;
		void upload(const std::shared_ptr<Texture2D>& textureToUpload, const void* data) const;

		void bindRootSignature(const RootSignature& rootSignature) const;
		void bindPipelineState(const PipelineState& pipelineState) const;
		void setTopology(PrimitiveTopology topology) const;
		void bindVertexBufferView(const VertexBufferView& view) const;
		void bindIndexBufferView(const IndexBufferView& view) const;
		void drawInstanced(
			UINT vertexCountPerInstance, 
			UINT instanceCount, 
			UINT startVertexLocation, 
			UINT startInstanceLocation) const;
		void drawIndexedInstanced(
			UINT indexCountPerInstance,
			UINT instanceCount,
			UINT startIndexLocation,
			INT baseVertexLocation,
			UINT startInstanceLocation) const;
		void setDescriptorHeaps(const std::vector<DescriptorHeap>& descriptorHeaps) const;
		void setGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHeap& descriptorHeap, UINT descriptorIndex) const;

		void transition(const std::shared_ptr<Resource>& resource, ResourceState afterState,
		                UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		                ResourceBarrierFlag::Type flags = ResourceBarrierFlag::None,
		                ResourceBarrierType type = ResourceBarrierType::Transition) const;
#pragma endregion

		// clears back buffer
		void clear() const;
		void postRecording() const;

		void flush() const;
		void moveToNextFrame() const;

		[[nodiscard]] const std::shared_ptr<Device>& getDevice() const;

		[[nodiscard]] uint32_t getFrameIndex() const;
		static UINT getFrameCount();

	private:
		bool m_bOnRenderLoop = false;

		std::shared_ptr<Device> m_device;

		std::unique_ptr<Swapchain> m_swapchain;
		std::unique_ptr<Viewport> m_viewPort;
		std::unique_ptr<ScissorRect> m_scissorRect;

		std::unique_ptr<GraphicsCommandList> m_graphicsCommandList;
		std::vector<std::unique_ptr<CommandAllocator>> m_graphicsCommandAllocators; // multi command allocators for multi buffering
		std::shared_ptr<CommandQueue> m_graphicsCommandQueue;

		std::unique_ptr<UploadContext> m_uploadContext;
#pragma region Imgui memebers
		bool m_bImguiInitiated= false;
		static std::unique_ptr<DescriptorHeap> s_imguiSrvDescriptorHeap;
		static constexpr UINT s_cImguiDescriptorCount = 32;
		static std::deque<UINT> s_imguiSrvDescriptorFreeIndices;
#pragma endregion
	};
}


