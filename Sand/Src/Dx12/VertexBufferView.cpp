#include "Snd/Pch.h"
#include "Snd/Dx12/VertexBufferView.h"

#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/GraphicsCommandList.h"


namespace Snd::Dx12
{
	VertexBufferView::VertexBufferView(const std::shared_ptr<Buffer>& buffer) : m_buffer(buffer)
	{
		m_bufferView.BufferLocation = buffer->getGpuVirtualAddress();
		m_bufferView.StrideInBytes = buffer->getElementSize();
		m_bufferView.SizeInBytes = buffer->getElementSize() * buffer->getElementCount();
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView::getDxBufferView() const
	{
		return m_bufferView;
	}
}