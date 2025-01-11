#include "Snd/Pch.h"
#include "Snd/Dx12/GraphicsCommandList.h"

#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/CommandAllocator.h"
#include "Snd/Dx12/DescriptorCpuHandle.h"
#include "Snd/Dx12/PipelineState.h"
#include "Snd/Dx12/RootSignature.h"
#include "Snd/Dx12/VertexBufferView.h"
#include "Snd/Dx12/DescriptorHeap.h"
#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/Viewport.h"
#include "Snd/Dx12/ScissorRect.h"
#include "Snd/Dx12/Texture.h"
#include "Snd/Dx12/IndexBufferView.h"
#include "Snd/Dx12/Utils/Common.h"
#include "Snd/Dx12/Utils/DxHelpers.h"

#include "Nmd/Logger.h"


using Microsoft::WRL::ComPtr;

namespace Snd::Dx12
{
	void memcpySubresource(
		const D3D12_MEMCPY_DEST* dest,
		const D3D12_SUBRESOURCE_DATA* src,
		const SIZE_T rowSizeInBytes,
		const UINT numRows,
		const UINT numSlices) noexcept
	{
		for (UINT z = 0; z < numSlices; ++z)
		{
			const auto pDestSlice = static_cast<BYTE*>(dest->pData) + dest->SlicePitch * z;
			const auto pSrcSlice = static_cast<const BYTE*>(src->pData) + src->SlicePitch * static_cast<LONG_PTR>(z);
			for (UINT y = 0; y < numRows; ++y)
			{
				memcpy(pDestSlice + dest->RowPitch * y,
					   pSrcSlice + src->RowPitch * static_cast<LONG_PTR>(y),
					   rowSizeInBytes);
			}
		}
	}

	GraphicsCommandList::GraphicsCommandList(const std::string& name, const Device& device, const CommandAllocator& allocator, GraphicsCommandListType type)
		: Object(name)
	{
		ThrowIfFailed(device.getDxDevice()->CreateCommandList(0, static_cast<D3D12_COMMAND_LIST_TYPE>(type),
			allocator.getDxAllocator().Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

		ThrowIfFailed(m_commandList->SetName(stringToWstr(m_name).c_str()));
	}

	void GraphicsCommandList::close() const
	{
		ThrowIfFailed(m_commandList->Close());
	}

	void GraphicsCommandList::reset(const CommandAllocator& allocator) const
	{
		ThrowIfFailed(m_commandList->Reset(allocator.getDxAllocator().Get(), nullptr));
	}

	void GraphicsCommandList::clearRenderTargetView(const DescriptorCpuHandle& cpuHandle, const FLOAT colorRgba[4]) const
	{
		m_commandList->ClearRenderTargetView(cpuHandle, colorRgba, 0, nullptr);
	}

	void GraphicsCommandList::setRenderTarget(const DescriptorCpuHandle& cpuHandle) const
	{
		m_commandList->OMSetRenderTargets(1, &cpuHandle, FALSE, nullptr);
	}

	void GraphicsCommandList::setTopologyTypeTriangleList(PrimitiveTopology topology) const
	{
		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void GraphicsCommandList::setViewports(const Viewport& viewport) const
	{
		m_commandList->RSSetViewports(1, &viewport);
	}

	void GraphicsCommandList::setScissorRects(const ScissorRect& scissorRect) const
	{
		m_commandList->RSSetScissorRects(1, &scissorRect);
	}

	void GraphicsCommandList::bindPipelineState(const PipelineState& pipelineState) const
	{
		m_commandList->SetPipelineState(pipelineState.getDxPipelineState().Get());
	}

	void GraphicsCommandList::bindVertexBufferView(const VertexBufferView& view) const
	{
		const auto dxBufferView = view.getDxBufferView();
		m_commandList->IASetVertexBuffers(0, 1, &dxBufferView);
	}

	void GraphicsCommandList::bindIndexBufferView(const IndexBufferView &view) const
	{
		const auto dxBufferView = view.getDxBufferView();
		m_commandList->IASetIndexBuffer(&dxBufferView);
	}

	void GraphicsCommandList::bindRootSignature(const RootSignature& rootSignature) const
	{
		const auto dxRoot = rootSignature.getDxRootSignature();
		m_commandList->SetGraphicsRootSignature(dxRoot.Get());
	}

	void GraphicsCommandList::setDescriptorHeaps(const std::vector<DescriptorHeap> &descriptorHeaps) const
	{
		std::vector<ID3D12DescriptorHeap*> dxHeaps(descriptorHeaps.size());
		for (size_t i = 0; i < descriptorHeaps.size(); ++i)
		{
			dxHeaps[i] = descriptorHeaps[i].getDxDescriptorHeap().Get();
		}

		m_commandList->SetDescriptorHeaps(static_cast<UINT>(dxHeaps.size()), dxHeaps.data());
	}

	void GraphicsCommandList::setGraphicsRootDescriptorTable(const UINT rootParameterIndex,
	                                                         const DescriptorHeap &descriptorHeap, const UINT descriptorIndex) const
	{
		m_commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, descriptorHeap.hGpu(descriptorIndex));
	}

	void GraphicsCommandList::copyBufferRegion(const Buffer &src, const Buffer &dst, const uint64_t srcOffset,
		const uint64_t dstOffset, const uint64_t bufferSize) const
	{
		m_commandList->CopyBufferRegion(
			dst.getDxResource().Get(),
			dstOffset,
			src.getDxResource().Get(),
			srcOffset,
			bufferSize);
	}

	void GraphicsCommandList::updateSubResources(const Buffer &intermediateBuffer, const Texture2D &destTexture,
		const UINT firstSubResource, const UINT numSubResources, const UINT64 intermediateOffset,
		const D3D12_SUBRESOURCE_DATA* subResourceData) const
	{
		const auto memToAlloc = static_cast<UINT64>(
			sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) +
			sizeof(UINT) +
			sizeof(UINT64)
		) * numSubResources;
		NOMAD_ASSERT(Nmd::AssertType::Assert, memToAlloc <= SIZE_MAX, "Could not allocate memory for texture {}", destTexture.getName());
		void* pMem = HeapAlloc(GetProcessHeap(), 0, memToAlloc);
		NOMAD_ASSERT(Nmd::AssertType::Assert, pMem != nullptr, "Could not allocate memory for texture {}", destTexture.getName());

		const auto layout = static_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
		const auto rowSizeInBytes = reinterpret_cast<UINT64*>(layout + numSubResources);
		const auto numRows = reinterpret_cast<UINT*>(rowSizeInBytes + numSubResources);

		const auto desc = destTexture.getDxResource()->GetDesc();

		UINT64 requiredSize = 0;
		ID3D12Device* pDevice = nullptr;
		ThrowIfFailed(destTexture.getDxResource()->GetDevice(IID_ID3D12Device, reinterpret_cast<void**>(&pDevice)));
		pDevice->GetCopyableFootprints(&desc,
			firstSubResource, numSubResources, intermediateOffset,
			layout, numRows, rowSizeInBytes, &requiredSize);
		pDevice->Release();

		updateSubResources(
			intermediateBuffer, destTexture, firstSubResource,
			numSubResources, requiredSize, layout,
			numRows, rowSizeInBytes, subResourceData);

		HeapFree(GetProcessHeap(), 0, pMem);
	}

	void GraphicsCommandList::drawInstanced(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation) const
	{
		m_commandList->DrawInstanced(
			vertexCountPerInstance,
			instanceCount,
			startVertexLocation,
			startInstanceLocation);
	}

	void GraphicsCommandList::drawIndexedInstanced(const UINT indexCountPerInstance, const UINT instanceCount,
		const UINT startIndexLocation, const INT baseVertexLocation, const UINT startInstanceLocation) const
	{
		m_commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList::getDxGraphicsCommandList() const
	{
		return m_commandList;
	}

	void GraphicsCommandList::updateSubResources(const Buffer &intermediateBuffer, const Texture2D &destTexture,
		const UINT firstSubresource, const UINT numSubResources, const UINT64 requiredSize,
		const D3D12_PLACED_SUBRESOURCE_FOOTPRINT *layouts, const UINT *numRows, const UINT64 *rowSizesInBytes,
		const D3D12_SUBRESOURCE_DATA* subResData) const
	{
		BYTE* pData;
		intermediateBuffer.map(0, 0, reinterpret_cast<void**>(&pData));

		for (UINT i = 0; i < numSubResources; ++i)
		{
			NOMAD_ASSERT(Nmd::AssertType::Assert, rowSizesInBytes[i] <= static_cast<SIZE_T>(-1),
				"rowSizesInBytes {}, has passed the value limit could cant create sub resource {} for texture {}",
				i, i, destTexture.getName());
			D3D12_MEMCPY_DEST DestData = { pData + layouts[i].Offset, layouts[i].Footprint.RowPitch, static_cast<SIZE_T>(layouts[i].Footprint.RowPitch) * static_cast<SIZE_T>(numRows[i]) };
			memcpySubresource(&DestData, &subResData[i], static_cast<SIZE_T>(rowSizesInBytes[i]), numRows[i], layouts[i].Footprint.Depth);
		}
		intermediateBuffer.unmap();

		for (UINT i = 0; i < numSubResources; ++i)
		{
			D3D12_TEXTURE_COPY_LOCATION dst{};
			dst.pResource = destTexture.getDxResource().Get();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.PlacedFootprint = {};
			dst.SubresourceIndex = i + firstSubresource;

			D3D12_TEXTURE_COPY_LOCATION src{};
			src.pResource = intermediateBuffer.getDxResource().Get();
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint = layouts[i];

			m_commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		}
	}
}
