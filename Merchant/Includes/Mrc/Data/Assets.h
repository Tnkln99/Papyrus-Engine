#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>

namespace Mrc
{
    struct AVertex
    {
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_normal;
        DirectX::XMFLOAT2 m_uv;
    };

    struct AStaticMesh
    {
        std::string m_name;
        std::vector<AVertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };

    struct AStaticModel
    {
        std::string m_name;
        std::vector<AStaticMesh> m_meshes;
    };

    struct AScene
    {
        std::string m_name;
        std::vector<AStaticModel> m_models;
    };
}
