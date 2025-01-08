#include "Snd/Pch.h"
#include "Snd/Dx12/Buffer.h"

#include "Snd/Dx12/Utils/DxHelpers.h"

#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/GraphicsCommandList.h"

namespace Snd::Dx12
{
    Buffer::Buffer
    (   
        const std::string& name,
        const Device& device,
        ResourceHeapType heapType,
        ResourceFlag::Type flags,
        const ResourceState initialState,
        const UINT elementSize,
        const UINT elementCount
    )
		: Resource(name, initialState, heapType, flags), m_elementSize{ elementSize }, m_elementCount{ elementCount }
    {
        const UINT64 bufferSize = static_cast<UINT64>(m_elementSize) * m_elementCount;

        // Create the default heap resource
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = static_cast<D3D12_HEAP_TYPE>(heapType);
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resDesc = {};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Alignment = 0;
        resDesc.Width = bufferSize;
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.Format = DXGI_FORMAT_UNKNOWN;
        resDesc.SampleDesc.Count = 1;
        resDesc.SampleDesc.Quality = 0;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resDesc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(m_flags);

        // Create the buffer resource in the default heap
        ThrowIfFailed(device.getDxDevice()->CreateCommittedResource
        (
            &heapProps,
            static_cast<D3D12_HEAP_FLAGS>(flags),
            &resDesc,
            static_cast<D3D12_RESOURCE_STATES>(m_state),
            nullptr,
            IID_PPV_ARGS(&m_resource)
        ));

        ThrowIfFailed(m_resource->SetName(stringToWstr(m_name).c_str()));
    }

    Buffer::~Buffer() = default;
}
