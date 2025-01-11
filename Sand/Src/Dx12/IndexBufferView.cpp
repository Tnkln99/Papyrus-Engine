#include "Snd/Pch.h"
#include "Snd/Dx12/IndexBufferView.h"

#include "Snd/Dx12/Buffer.h"

namespace Snd::Dx12
{
    IndexBufferView::IndexBufferView(const std::shared_ptr<Buffer> &buffer) : m_buffer(buffer)
    {
        m_indexBufferView.BufferLocation = buffer->getGpuVirtualAddress();
        m_indexBufferView.SizeInBytes = buffer->getElementCount() * buffer->getElementSize();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;  // Use DXGI_FORMAT_R32_UINT for 32-bit indices
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView::getDxBufferView() const
    {
        return m_indexBufferView;
    }
}
