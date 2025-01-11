#pragma once

#include "Snd/Dx12/Object.h"

#include <vector>

namespace Snd::Dx12
{
	class IndexBufferView;
	enum class PrimitiveTopology;

	class Texture2D;
	class DescriptorHeap;
	class Device;
	class CommandAllocator;
	class PipelineState;
	class RootSignature;
	class VertexBufferView;
	class Buffer;

	struct Viewport;
	struct ScissorRect;

	struct DescriptorCpuHandle;

	enum class GraphicsCommandListType
	{
		// Specifies a command buffer that the GPU can execute
		Direct = D3D12_COMMAND_LIST_TYPE_DIRECT,
		// Specifies a command buffer that can be executed only directly via a direct command list.
		Bundle = D3D12_COMMAND_LIST_TYPE_BUNDLE,
		// Specifies a command buffer for computing.
		Compute = D3D12_COMMAND_LIST_TYPE_COMPUTE,
		// Specifies a command buffer for copying.
		Copy = D3D12_COMMAND_LIST_TYPE_COPY,
	};

	class GraphicsCommandList : public Object
	{
	public:
		GraphicsCommandList(const std::string& name, const Device& device, const CommandAllocator& allocator, GraphicsCommandListType type);

		void close() const;
		void reset(const CommandAllocator& allocator) const;

		void clearRenderTargetView(const DescriptorCpuHandle& cpuHandle, const FLOAT ColorRGBA[4]) const;

		void setRenderTarget(const DescriptorCpuHandle& cpuHandle) const;
		void setTopologyTypeTriangleList(PrimitiveTopology topology) const;
		void setViewports(const Viewport& viewport) const;
		void setScissorRects(const ScissorRect& scissorRect) const;

		void bindPipelineState(const PipelineState& pipelineState) const;
		void bindVertexBufferView(const VertexBufferView& view) const;
		void bindIndexBufferView(const IndexBufferView& view) const;
		void bindRootSignature(const RootSignature& rootSignature) const;
		void setDescriptorHeaps(const std::vector<DescriptorHeap>& descriptorHeaps) const;
		void setGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHeap& descriptorHeap, UINT descriptorIndex) const;
		void copyBufferRegion(const Buffer& src, const Buffer& dst, uint64_t srcOffset, uint64_t dstOffset, uint64_t bufferSize) const;

		void updateSubResources(const Buffer& intermediateBuffer, const Texture2D& destTexture,
			UINT firstSubResource, UINT numSubResources, UINT64 intermediateOffset, const D3D12_SUBRESOURCE_DATA* subResourceData) const;

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


		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> getDxGraphicsCommandList() const;

	private:
		void updateSubResources( const Buffer& intermediateBuffer, const Texture2D& destTexture,
			UINT firstSubresource,
			UINT numSubResources,
			UINT64 requiredSize,
			const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts,
			const UINT* numRows,
			const UINT64* rowSizesInBytes,
			const D3D12_SUBRESOURCE_DATA* subResData) const;

	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	};
}


