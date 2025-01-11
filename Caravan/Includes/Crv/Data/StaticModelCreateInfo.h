#pragma once

#include <string>
#include <vector>

namespace Crv
{
    struct GeometryBufferCreateInfo
    {
        void* m_vertexBuffer;
        UINT m_vertexSize;
        UINT m_vertexCount;

        std::vector<uint16_t> m_indices;
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
