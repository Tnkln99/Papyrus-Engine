#pragma once

#include "Snd/Dx12/Object.h"

namespace Snd::Dx12
{
    class CommandQueue;
    class Device;

    class Fence : public Object
    {
    public:
	    explicit Fence(const std::string& name, const Device& device);

		void scheduleSignalCommand(const CommandQueue& commandQueue, UINT64 fenceValue) const;

        // returns true if fence has came to the value provided 
        [[nodiscard]] bool checkForWait(UINT64 fenceValue) const;
        void waitForGpu(UINT64 fenceValue) const;
    private:
        HANDLE m_fenceEvent;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    };
}