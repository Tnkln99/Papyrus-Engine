#pragma once

#include <string>

#include "Resource.h"

namespace Snd::Dx12
{
    class Device;
    class GraphicsCommandList;
    class CommandQueue;
    class Fence;

	struct UploadContext;

    class Buffer final : public Resource
    {
    public:
        // Buffers will be created in COMMON resource state 
        Buffer(
            const std::string& name,
            const Device& device,
            ResourceHeapType heapType,
            ResourceFlag::Type flags,
            ResourceState initialState,
            UINT elementSize, 
            UINT elementCount);

		~Buffer();

        [[nodiscard]] UINT getElementSize() const { return m_elementSize; }
        [[nodiscard]] UINT getElementCount() const { return m_elementCount; }
    private:
        UINT m_elementSize;
        UINT m_elementCount;
    };
}
