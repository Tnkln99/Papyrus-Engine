#include "Snd/Pch.h"
#include "Snd/Dx12/Fence.h"

#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/CommandQueue.h"

#include "Snd/Dx12/Utils/DxHelpers.h"

using Microsoft::WRL::ComPtr;

namespace Snd::Dx12
{
	Fence::Fence(const std::string& name, const Device& device) : Object(name)
	{
        ThrowIfFailed(device.getDxDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

	    ThrowIfFailed(m_fence->SetName(stringToWstr(m_name).c_str()));
	}

    void Fence::scheduleSignalCommand(const CommandQueue& commandQueue, const UINT64 fenceValue) const
    {
		ThrowIfFailed(commandQueue.getDxCommandQueue()->Signal(m_fence.Get(), fenceValue));
    }

    bool Fence::checkForWait(const UINT64 fenceValue) const
    {
        return m_fence->GetCompletedValue() < fenceValue;
    }

    void Fence::waitForGpu(const UINT64 fenceValue) const
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}
