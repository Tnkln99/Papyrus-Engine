#pragma once

#include "Snd/Dx12/Object.h"

#include <string>

namespace Snd::Dx12
{
	class Device;
	class GraphicsCommandList;
	enum class GraphicsCommandListType;

	class CommandQueue : public Object
	{
	public:
		enum class Priority
		{
			Normal = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			High = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
			GlobalRealTime = D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME,
		};

		struct Flag
		{
			enum Type : unsigned int
			{
				None = D3D12_COMMAND_QUEUE_FLAG_NONE,
				DisableGpuTimeOut = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT,
			};
		};

		CommandQueue(const std::string& name, const Device& device, GraphicsCommandListType commandListType, Priority priority = Priority::Normal, Flag::Type flag = Flag::None);
		void executeSingleCommandList(const GraphicsCommandList& commandListsToExecute)  const;

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> getDxCommandQueue() const;
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	};
}
