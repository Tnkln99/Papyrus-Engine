#include "Snd/Pch.h"
#include "Snd/Dx12/CommandAllocator.h"

#include "Snd/Dx12/GraphicsCommandList.h"
#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/Utils/DxHelpers.h"

namespace Snd::Dx12
{
	CommandAllocator::CommandAllocator(const std::string &name, const Device& device, GraphicsCommandListType commandListType)
	: Object(name),  m_type { commandListType }
	{
		ThrowIfFailed(device.getDxDevice()->CreateCommandAllocator(static_cast<D3D12_COMMAND_LIST_TYPE>(commandListType), IID_PPV_ARGS(&m_commandAllocator)));
		ThrowIfFailed(m_commandAllocator->SetName(stringToWstr(m_name).c_str()));
	}

	void CommandAllocator::reset() const
	{
		ThrowIfFailed(m_commandAllocator->Reset());
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator::getDxAllocator() const
	{
		return m_commandAllocator;
	}
}
