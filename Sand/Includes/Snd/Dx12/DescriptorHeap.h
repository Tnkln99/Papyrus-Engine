#pragma once

#include "Snd/Dx12/DescriptorCpuHandle.h"
#include "Snd/Dx12/DescriptorGpuHandle.h"
#include "Snd/Dx12/Object.h"

#include <string>

namespace Snd::Dx12
{
	class Texture2D;
	class Buffer;
	class Device;

	enum class DescriptorHeapType
	{
		CbvSrvUav = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,		// Constant Buffer View, Shader Resource View, Unordered Access View
		Sampler = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,			// Sampler
		Rtv = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,					// Render Target View
		Dsv = D3D12_DESCRIPTOR_HEAP_TYPE_DSV					// Depth Stencil View
	};

	// https://learn.microsoft.com/en-us/windows/win32/direct3d12/creating-descriptor-heaps
	class DescriptorHeap : public Object
	{
	public:
		DescriptorHeap(const std::string& name, const Device& device, DescriptorHeapType type,
			UINT numDescriptors,
			bool bShaderVisible = false);

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> getDxDescriptorHeap() const;

		[[nodiscard]] DescriptorCpuHandle hCpu(UINT index) const;
		[[nodiscard]] DescriptorGpuHandle hGpu(UINT index) const;

		// todo: add other descriptor initiations
		void newConstantBufferView(const Device& device, const Buffer& buffer, UINT index) const;
		void newShaderResourceView(const Device& device, const Texture2D& texture, UINT index) const;
		void newShaderResourceView(const Device& device, const Buffer& buffer, UINT index) const;
	private:
		D3D12_DESCRIPTOR_HEAP_DESC m_desc{};
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descHeap;

		DescriptorCpuHandle m_cpuHeapStart;
		DescriptorGpuHandle m_gpuHeapStart;

		UINT m_handleIncrementSize;
	};
}
