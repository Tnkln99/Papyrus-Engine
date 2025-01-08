#pragma once

#include "Snd/Dx12/Object.h"

#include <string>

namespace Snd::Dx12
{
	class Device : public Object
	{
	public:
		explicit Device(const std::string& name);
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Device> getDxDevice() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> m_device = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter> m_adaptor = nullptr;

	private:
		static void CALLBACK debugCallback(
			D3D12_MESSAGE_CATEGORY category,
			D3D12_MESSAGE_SEVERITY severity,
			D3D12_MESSAGE_ID id,
			LPCSTR pDescription,
			void* pContext);

		void enableDebugMessages() const;

		void printAdaptorDesc(DXGI_ADAPTER_DESC desc) const;

		static void getHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
	};
}
