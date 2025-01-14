#include "Crv/Pch.h"
#include "Crv/PrivateData/StaticModel.h"

#include "Crv/Data/StaticModelCreateInfo.h"

#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/VertexBufferView.h"
#include "Snd/Dx12/IndexBufferView.h"
#include "Snd/Dx12/Context.h"


namespace Crv
{
    StaticMesh::StaticMesh(const Snd::Dx12::Context& graphicsContext, const std::string& name, const GeometryBufferCreateInfo& geometryInfo)
    {
        {
            std::string vertexBufferName = name + "VertexBuffer";
            m_vertexBuffer = std::make_shared<Snd::Dx12::Buffer>
            (
                vertexBufferName,
                *graphicsContext.getDevice(),
                Snd::Dx12::ResourceHeapType::Default,
                Snd::Dx12::ResourceFlag::None,
                Snd::Dx12::ResourceState::Common,
                geometryInfo.m_vertexSize,
                geometryInfo.m_vertexCount
            );
            m_vertexBufferView = std::make_unique<Snd::Dx12::VertexBufferView>(m_vertexBuffer);
            m_vertexCpuData = geometryInfo.m_vertexBuffer;
        }

        {
            std::string indexBufferName = name + "IndexBuffer";
            m_indexBuffer = std::make_shared<Snd::Dx12::Buffer>
            (
                indexBufferName,
                *graphicsContext.getDevice(),
                Snd::Dx12::ResourceHeapType::Default,
                Snd::Dx12::ResourceFlag::None,
                Snd::Dx12::ResourceState::Common,
                geometryInfo.m_vertexSize,
                geometryInfo.m_vertexCount
            );
            m_indexBufferView = std::make_unique<Snd::Dx12::IndexBufferView>(m_indexBuffer);
            m_indexCpuData = geometryInfo.m_indices;
        }

    }

    StaticMesh::~StaticMesh() = default;

    const std::shared_ptr<Snd::Dx12::VertexBufferView>& StaticMesh::getVertexBufferView() const
    {
        return m_vertexBufferView;
    }

    const std::shared_ptr<Snd::Dx12::Buffer>& StaticMesh::getVertexBuffer() const
    {
        return m_vertexBuffer;
    }

    const void* StaticMesh::getVertexBufferCpuData() const
    {
        return m_vertexCpuData;
    }

    const std::shared_ptr<Snd::Dx12::IndexBufferView>& StaticMesh::getIndexBufferView() const
    {
        return m_indexBufferView;
    }

    const std::shared_ptr<Snd::Dx12::Buffer>& StaticMesh::getIndexBuffer() const
    {
        return m_indexBuffer;
    }

    const std::vector<uint32_t> & StaticMesh::getIndexBufferCpuData() const
    {
        return m_indexCpuData;
    }
}
