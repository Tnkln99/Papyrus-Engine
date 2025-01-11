#pragma once

#include <string>
#include <memory>
#include <vector>

namespace Snd::Dx12
{
	class IndexBufferView;
	class Buffer;
	class VertexBufferView;
	class Context;
}

namespace Crv
{
	struct GeometryBufferCreateInfo;

	class StaticMesh
	{
	public:
		StaticMesh(const Snd::Dx12::Context& graphicsContext, const std::string& name, const GeometryBufferCreateInfo& geometryInfo);
		~StaticMesh();

		[[nodiscard]] std::shared_ptr<Snd::Dx12::VertexBufferView> getVertexBufferView() const;
		std::shared_ptr<Snd::Dx12::Buffer> getVertexBuffer();
		[[nodiscard]] void* getVertexBufferCpuData() const;

		[[nodiscard]] std::shared_ptr<Snd::Dx12::IndexBufferView> getIndexBufferView() const;
		std::shared_ptr<Snd::Dx12::Buffer> getIndexBuffer();
		[[nodiscard]] const std::vector<uint16_t>& getIndexBufferCpuData() const;

	private:
		void* m_vertexCpuData;
		std::shared_ptr<Snd::Dx12::Buffer> m_vertexBuffer;
		std::shared_ptr<Snd::Dx12::VertexBufferView> m_vertexBufferView;

		std::vector<uint16_t> m_indexCpuData;
		std::shared_ptr<Snd::Dx12::Buffer> m_indexBuffer;
		std::shared_ptr<Snd::Dx12::IndexBufferView> m_indexBufferView;
	};

	struct StaticModel
	{
		std::string m_name;
		std::vector<StaticMesh> m_meshes;
	};
}

