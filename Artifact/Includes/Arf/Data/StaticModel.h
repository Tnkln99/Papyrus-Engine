#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

namespace Arf
{
    struct Vertex
    {
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_normal;
        DirectX::XMFLOAT2 m_uv;
    };

    struct StaticMesh
    {
        std::string m_name;
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };

    struct StaticModel
    {
        std::string m_name;
        std::vector<StaticMesh> m_meshes;
    };
}