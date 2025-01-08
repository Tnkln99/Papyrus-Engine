#pragma once

namespace Snd::Dx12
{
    enum class ShaderVisibility
    {
        All = D3D12_SHADER_VISIBILITY_ALL,
        Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
        Pixel = D3D12_SHADER_VISIBILITY_PIXEL,
        Amplification = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
        Mesh = D3D12_SHADER_VISIBILITY_MESH,
    };

    enum class PrimitiveTopology
    {
        TriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        PointList = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
    };
}