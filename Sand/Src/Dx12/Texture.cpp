#include "Snd/Pch.h"
#include "Snd/Dx12/Texture.h"

#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/Swapchain.h"
#include "Snd/Dx12/DescriptorHeap.h"

#include "Snd/Dx12/Utils/DxHelpers.h"

#include <string>

#include "Snd/Dx12/Buffer.h"

namespace Snd::Dx12
{
	Texture2D::Texture2D(
        const std::string& name,
        const Device& device,
        const ResourceHeapType heapType,
        const ResourceFlag::Type flags,
        const UINT width,
        const UINT height,
        const TextureFormat format,
        const bool doesUseClearValue,
        const ResourceState initialState,
        const UINT16 mipLevels,
        const UINT16 arraySize,
        const UINT sampleCount
    )
        : Resource(name, initialState, heapType, flags),
          m_width(width),
          m_height(height),
          m_format(format),
          m_mipLevels(mipLevels),
          m_arraySize(arraySize),
          m_sampleCount(sampleCount)
    {
        // Describe the texture resource
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Alignment = 0;
        texDesc.Width = static_cast<UINT64>(m_width);
        texDesc.Height = m_height;
        texDesc.DepthOrArraySize = m_arraySize;
        texDesc.MipLevels = m_mipLevels;
        texDesc.Format = static_cast<DXGI_FORMAT>(m_format);
        texDesc.SampleDesc.Count = m_sampleCount;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // Typically, use UNKNOWN for textures
        texDesc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(m_flags);

        // Set up heap properties
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = static_cast<D3D12_HEAP_TYPE>(m_heapType);
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        if (!doesUseClearValue)
        {
        	ThrowIfFailed(device.getDxDevice()->CreateCommittedResource(
				&heapProps,
				static_cast<D3D12_HEAP_FLAGS>(m_heapFlags),
				&texDesc,
				static_cast<D3D12_RESOURCE_STATES>(m_state),
				nullptr,
				IID_PPV_ARGS(&m_resource)
			));
        }
		else
		{
			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = static_cast<DXGI_FORMAT>(m_format);
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;

			ThrowIfFailed(device.getDxDevice()->CreateCommittedResource(
				&heapProps,
				static_cast<D3D12_HEAP_FLAGS>(m_heapFlags),
				&texDesc,
				static_cast<D3D12_RESOURCE_STATES>(m_state),
				&depthOptimizedClearValue,
				IID_PPV_ARGS(&m_resource)
			));
		}


        ThrowIfFailed(m_resource->SetName(stringToWstr(m_name).c_str()));
    }

    UINT Texture2D::getFormatByteSize(const TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8G8B8A8Unorm:
        case TextureFormat::B8G8R8A8Unorm:
        case TextureFormat::R8G8B8A8UnormSrgb:
        case TextureFormat::B8G8R8A8UnormSrgb:
            return 4;

        case TextureFormat::R16G16B16A16Float:
            return 8;

        case TextureFormat::R32G32B32A32Float:
            return 16;
        case TextureFormat::D32Float:
        case TextureFormat::D24UnormS8Uint:
            return 4;
        default:
            NOMAD_ASSERT(Nmd::AssertType::Assert, false, "Unknown DXGI_FORMAT");
            return 0;
        }
    }

    UINT64 Texture2D::getRequiredIntermediateSize(const UINT firstSubResource, const UINT numSubResources) const
	{
	    const auto Desc = m_resource->GetDesc();

	    UINT64 requiredSize = 0;

	    ID3D12Device* pDevice = nullptr;
	    ThrowIfFailed(m_resource->GetDevice(IID_ID3D12Device, reinterpret_cast<void**>(&pDevice)));

		// this defines the padding
		// requiredSize has the padding inside
	    pDevice->GetCopyableFootprints(&Desc, firstSubResource, numSubResources, 0, nullptr, nullptr, nullptr, &requiredSize);
	    pDevice->Release();

	    return requiredSize;
	}

    SwapchainRenderTarget::SwapchainRenderTarget(const Device& device, const DescriptorHeap& rtvHeap, Swapchain& swapchain, const UINT bufferIndex) : Resource(), m_swapchain{ swapchain }
	{
        ThrowIfFailed(m_swapchain.getDxSwapchain()->GetBuffer(bufferIndex, IID_PPV_ARGS(&m_resource)));
		m_flags = static_cast<ResourceFlag::Type>(m_resource->GetDesc().Flags);
		m_state = ResourceState::Present;
		m_name = "SwapchainBuffer" + std::to_string(bufferIndex);

        device.getDxDevice()->CreateRenderTargetView(m_resource.Get(), nullptr, rtvHeap.hCpu(bufferIndex));

		const std::wstring wName = stringToWstr(m_name);
        const LPCWSTR namePtr = wName.c_str();

        ThrowIfFailed(m_resource->SetName(namePtr));
	}
}
