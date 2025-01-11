#pragma once

#include <memory>

namespace Snd::Dx12
{
    class Buffer;

    class IndexBufferView
    {
    public:
        explicit IndexBufferView(const std::shared_ptr<Buffer>& buffer);

        [[nodiscard]] D3D12_INDEX_BUFFER_VIEW getDxBufferView() const;
    private:
        // this is here for debug information only
        std::weak_ptr<Buffer> m_buffer;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
    };
}


