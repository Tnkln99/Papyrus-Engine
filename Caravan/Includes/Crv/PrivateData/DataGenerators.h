#pragma once

#include <vector>
#include <Windows.h>

namespace Crv::Generator
{
    struct alignas(16) Vertex  {
        DirectX::XMFLOAT4 m_position;
        DirectX::XMFLOAT2 m_uv;
    };

    struct Mesh {
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };

    inline std::vector<UINT8> generateTextureData(const UINT textureWidth, const UINT textureHeight, const UINT texturePixelSize)
    {
        const UINT rowPitch = textureWidth * texturePixelSize;
        const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
        const UINT cellHeight = textureWidth >> 3;    // The height of a cell in the checkerboard texture.
        const UINT textureSize = rowPitch * textureHeight;

        std::vector<UINT8> data(textureSize);
        UINT8* pData = &data[0];

        for (UINT n = 0; n < textureSize; n += texturePixelSize)
        {
            const UINT x = n % rowPitch;
            const UINT y = n / rowPitch;
            const UINT i = x / cellPitch;

            if (const UINT j = y / cellHeight; i % 2 == j % 2)
            {
                pData[n] = 0x00;        // R
                pData[n + 1] = 0x00;    // G
                pData[n + 2] = 0x00;    // B
                pData[n + 3] = 0xff;    // A
            }
            else
            {
                pData[n] = 0xff;        // R
                pData[n + 1] = 0xff;    // G
                pData[n + 2] = 0xff;    // B
                pData[n + 3] = 0xff;    // A
            }
        }

        return data;
    }

    inline Mesh createRectangle() {
        Mesh mesh;

        // Define the vertices of the rectangle
        constexpr Vertex v0 = {
            { -0.5f,  0.5f, 0.0f, 1.0f }, // Position
            {  0.0f,  0.0f }  // UV
        };

        constexpr Vertex v1 = {
            {  0.5f,  0.5f, 0.0f, 1.0f }, // Position
            {  1.0f,  0.0f }  // UV
        };

        constexpr Vertex v2 = {
            { -0.5f, -0.5f, 0.0f, 1.0f }, // Position
            {  0.0f,  1.0f }  // UV
        };

        constexpr Vertex v3 = {
            {  0.5f, -0.5f, 0.0f, 1.0f }, // Position
            {  1.0f,  1.0f }  // UV
        };

        // Add vertices to the list
        mesh.m_vertices.push_back(v0);
        mesh.m_vertices.push_back(v1);
        mesh.m_vertices.push_back(v2);
        mesh.m_vertices.push_back(v3);

        // Define indices for two triangles
        mesh.m_indices = {
            0, 1, 2, // First triangle
            2, 1, 3  // Second triangle
        };

        return mesh;
    }
}

