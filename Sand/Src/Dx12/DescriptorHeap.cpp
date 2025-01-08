#include "Snd/Pch.h"
#include "Snd/Dx12/DescriptorHeap.h"

#include "Snd/Dx12/Device.h"

#include "Snd/Dx12/Utils/DxHelpers.h"

#include <cassert>

#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/Texture.h"

#include <Nmd/Asserter.h>

using Microsoft::WRL::ComPtr;

namespace Snd::Dx12
{
	DescriptorHeap::DescriptorHeap(const std::string& name, const Device& device, DescriptorHeapType type, const UINT numDescriptors, bool isShaderVisible)
		: Object(name)
	{
        m_desc.NumDescriptors = numDescriptors;
        m_desc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
        m_desc.Flags = (isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

        ThrowIfFailed(device.getDxDevice()->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&m_descHeap)));

        m_cpuHeapStart = m_descHeap->GetCPUDescriptorHandleForHeapStart();
        if(isShaderVisible)
        {
            m_gpuHeapStart = m_descHeap->GetGPUDescriptorHandleForHeapStart();
        }

        m_handleIncrementSize = device.getDxDevice()->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type));

		ThrowIfFailed(m_descHeap->SetName(stringToWstr(m_name).c_str()));
	}

    ComPtr<ID3D12DescriptorHeap> DescriptorHeap::getDxDescriptorHeap() const
    {
        return m_descHeap;
    }

    DescriptorCpuHandle DescriptorHeap::hCpu(const UINT index) const
    {
        DescriptorCpuHandle offsetHandle(m_cpuHeapStart);
        return offsetHandle.Offset(static_cast<INT>(index), m_handleIncrementSize);
    }

    DescriptorGpuHandle DescriptorHeap::hGpu(const UINT index) const
    {
        NOMAD_ASSERT(Nmd::AssertType::Assert, m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, "Cant cant Descriptor GPU Handle if not shader visible");

        DescriptorGpuHandle offsetHandle(m_gpuHeapStart);
        return offsetHandle.Offset(static_cast<INT>(index), m_handleIncrementSize);
    }

    void DescriptorHeap::newConstantBufferView(const Device &device, const Buffer &buffer, const UINT index) const
    {
		NOMAD_ASSERT(Nmd::AssertType::Assert, m_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "Descriptor type is not correct ");

	    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	    cbvDesc.BufferLocation = buffer.getGpuVirtualAddress();
	    cbvDesc.SizeInBytes = buffer.getElementCount() * buffer.getElementSize();

		device.getDxDevice()->CreateConstantBufferView(&cbvDesc, hCpu(index));
	}

    void DescriptorHeap::newShaderResourceView(const Device &device, const Texture2D &texture, const UINT index) const
	{
		NOMAD_ASSERT(Nmd::AssertType::Assert, m_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "Descriptor type is not correct ");

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = static_cast<DXGI_FORMAT>(texture.getFormat());
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texture.getSampleCount();

		device.getDxDevice()->CreateShaderResourceView(texture.getDxResource().Get(), &srvDesc, hCpu(index));
	}
}
