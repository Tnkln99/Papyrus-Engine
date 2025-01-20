#pragma once

#include <string>
#include <vector>

namespace Crv
{
    struct GeometryBufferCreateInfo
    {
        std::vector<uint8_t> m_vertexBuffer; // Stores the vertex buffer data
        UINT m_vertexSize;
        size_t m_vertexCount;

        std::vector<uint32_t> m_indices;
    };

    struct StaticMeshCreateInfo
    {
        std::string m_name;
        GeometryBufferCreateInfo m_verticesInfo;
    };

    struct StaticModelCreateInfo
    {
        std::string m_name;
        std::vector<StaticMeshCreateInfo> m_meshes;
    };
}
