#include <utility>

#include "Snd/Pch.h"
#include "Snd/Dx12/Resource.h"

#include "Snd/Dx12/GraphicsCommandList.h"
#include "Snd/Dx12/Utils/DxHelpers.h"

#include <Nmd/Asserter.h>

namespace Snd::Dx12
{
	Resource::Resource(const std::string& name, const ResourceState state, const ResourceHeapType heaptype, const ResourceFlag::Type flags, const HeapFlag::Type heapFlag)
					: Object{std::move(name)}, m_heapType(heaptype), m_state{ state } ,m_flags{ flags }, m_heapFlags { heapFlag } {}

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource::getDxResource() const
	{
		return m_resource;
	}

	UINT64 Resource::getGpuVirtualAddress() const
	{
		return m_resource->GetGPUVirtualAddress();
	}

	void Resource::map(const SIZE_T begin, const SIZE_T end, void **ppData) const
	{
		const D3D12_RANGE range{begin, end};
		ThrowIfFailed(m_resource->Map(0, &range, ppData));
	}

	void Resource::unmap() const
	{
		m_resource->Unmap(0, nullptr);
	}

	void Resource::transition(
		const GraphicsCommandList& commandList, 
		ResourceState afterState,
		const UINT subresource,
		ResourceBarrierFlag::Type flags,
		ResourceBarrierType type)
	{
		D3D12_RESOURCE_BARRIER result = {};
		D3D12_RESOURCE_BARRIER& barrier = result;

		result.Type = static_cast<D3D12_RESOURCE_BARRIER_TYPE>(type);
		result.Flags = static_cast<D3D12_RESOURCE_BARRIER_FLAGS>(flags);

		barrier.Transition.pResource = m_resource.Get();
		barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(m_state);
		barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(afterState);
		barrier.Transition.Subresource = subresource;

		commandList.getDxGraphicsCommandList()->ResourceBarrier(1, &result);

		m_state = afterState;
	}
}
