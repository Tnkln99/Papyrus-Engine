#include "Snd/Pch.h"
#include "Snd/Dx12/CommandQueue.h"

#include "Snd/Dx12/GraphicsCommandList.h"
#include "Snd/Dx12/Device.h"

#include "Snd/Dx12/Utils/DxHelpers.h"

using Microsoft::WRL::ComPtr;

namespace Snd::Dx12
{
	CommandQueue::CommandQueue(const std::string& name, const Device& device, GraphicsCommandListType commandListType, Priority priority, Flag::Type flag)
		: Object(name)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = static_cast<D3D12_COMMAND_LIST_TYPE>(commandListType);
		queueDesc.Priority = static_cast<D3D12_COMMAND_QUEUE_PRIORITY>(priority);
		queueDesc.Flags = static_cast<D3D12_COMMAND_QUEUE_FLAGS>(flag);

		ThrowIfFailed(device.getDxDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		ThrowIfFailed(m_commandQueue->SetName(stringToWstr(m_name).c_str()));
	}

	void CommandQueue::executeSingleCommandList(const GraphicsCommandList& commandListsToExecute) const
	{
		ID3D12CommandList* ppCommandLists[] = { commandListsToExecute.getDxGraphicsCommandList().Get() };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	ComPtr<ID3D12CommandQueue> CommandQueue::getDxCommandQueue() const
	{
		return m_commandQueue;
	}
}
