#pragma once

#include <memory>

namespace Snd::Dx12
{
	class Buffer;

	class VertexBufferView
	{
	public:
		explicit VertexBufferView(const std::shared_ptr<Buffer>& buffer);

		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW getDxBufferView() const;
	private:
		// this is here for debug information only
		std::weak_ptr<Buffer> m_buffer;
		D3D12_VERTEX_BUFFER_VIEW m_bufferView;
	};
}


