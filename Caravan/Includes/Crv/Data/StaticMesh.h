#pragma once

#include <string>
#include <memory>

namespace Snd::Dx12
{
	class Buffer;
	class VertexBufferView;
	class Context;
}

namespace Crv
{
	struct VerticesInfo;

	class StaticMesh
	{
	public:
		StaticMesh(const std::shared_ptr<Snd::Dx12::Context>& context, const std::string& name, const VerticesInfo& verticesInfo);
		~StaticMesh();

		void immediateUpload() const;
		void bind() const;
	private:
		// cpu data
		void* m_data;
		std::weak_ptr<Snd::Dx12::Context> m_context;

		std::shared_ptr<Snd::Dx12::Buffer> m_vertexBuffer;
		std::unique_ptr<Snd::Dx12::VertexBufferView> m_vertexBufferView;
	};
}

