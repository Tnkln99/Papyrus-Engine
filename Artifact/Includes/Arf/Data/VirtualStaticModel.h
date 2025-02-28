#pragma once

#include <string>
#include <vector>

namespace Arf
{
    struct VertexDescriptor
    {
        enum EType : uint32_t
        {
            Position,
            Normal,
            TexCoord,
            Tangent,
            Bitangent,
            Count
        };

        EType       m_type;
        uint32_t    m_offset;
    };

    struct Cluster
    {
        uint16_t m_id;
        std::vector<uint16_t> m_children;

        // todo: Cluster wont hold the geometrie in the future it will be chunk
        std::vector<uint8_t> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<VertexDescriptor> m_vertexDescriptor;
    };

    struct Dag
    {
        std::vector<Cluster> m_clusters;
    };

    struct VirtualStaticMesh
    {
        std::string m_name;
        Dag dag;
    };

    struct VirtualStaticModel
    {
        std::string m_name;
        std::vector<VirtualStaticMesh> m_staticMeshes;
    };
}
