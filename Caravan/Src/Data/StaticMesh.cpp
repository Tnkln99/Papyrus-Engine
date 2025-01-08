#include "Crv/Pch.h"
#include "Crv/Data/StaticMesh.h"

#include "Crv/Data/VerticesInfo.h"

#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/VertexBufferView.h"
#include "Snd/Dx12/Context.h"


namespace Crv
{
    StaticMesh::StaticMesh(const std::shared_ptr<Snd::Dx12::Context>& context, const std::string& name, const VerticesInfo& verticesInfo)
        : m_context { context }
    {
        m_vertexBuffer = std::make_shared<Snd::Dx12::Buffer>
            (
                name, 
                *context->getDevice(),
                Snd::Dx12::ResourceHeapType::Default,
                Snd::Dx12::ResourceFlag::None,
                Snd::Dx12::ResourceState::Common,
                verticesInfo.m_vertexSize,
                verticesInfo.m_vertexCount
            );

        m_vertexBufferView = std::make_unique<Snd::Dx12::VertexBufferView>(m_vertexBuffer);

		m_data = verticesInfo.m_vertexBuffer;
    }

    StaticMesh::~StaticMesh() = default;

    void StaticMesh::immediateUpload() const
    {
        m_context.lock()->upload(m_vertexBuffer, m_data);
    }

    void StaticMesh::bind() const
    {
        m_context.lock()->bindVertexBufferView(*m_vertexBufferView);
    }
}
