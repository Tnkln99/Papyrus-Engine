#pragma once

#include <vector>
#include <Windows.h>

namespace Crv::Generator
{
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
}

