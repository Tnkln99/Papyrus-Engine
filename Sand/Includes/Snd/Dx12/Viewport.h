#pragma once

namespace Snd::Dx12
{
    struct Viewport : public D3D12_VIEWPORT
    {
        Viewport() = default;
        explicit Viewport(const D3D12_VIEWPORT& o) noexcept :
            D3D12_VIEWPORT(o)
        {}
        explicit Viewport(
            const FLOAT topLeftX,
            const FLOAT topLeftY,
            const FLOAT width,
            const FLOAT height,
            const FLOAT minDepth = D3D12_MIN_DEPTH,
            FLOAT maxDepth = D3D12_MAX_DEPTH) noexcept
        {
            TopLeftX = topLeftX;
            TopLeftY = topLeftY;
            Width = width;
            Height = height;
            MinDepth = minDepth;
            MaxDepth = maxDepth;
        }
        explicit Viewport(
            _In_ ID3D12Resource* pResource,
            const UINT mipSlice = 0,
            const FLOAT topLeftX = 0.0f,
            const FLOAT topLeftY = 0.0f,
            const FLOAT minDepth = D3D12_MIN_DEPTH,
            const FLOAT maxDepth = D3D12_MAX_DEPTH) noexcept
        {
#if defined(_MSC_VER) || !defined(_WIN32)
            const auto Desc = pResource->GetDesc();
#else
            D3D12_RESOURCE_DESC tmpDesc;
            const auto& Desc = *pResource->GetDesc(&tmpDesc);
#endif
            const UINT64 SubresourceWidth = Desc.Width >> mipSlice;
            const UINT64 SubresourceHeight = Desc.Height >> mipSlice;
            switch (Desc.Dimension)
            {
            case D3D12_RESOURCE_DIMENSION_BUFFER:
                TopLeftX = topLeftX;
                TopLeftY = 0.0f;
                Width = float(Desc.Width) - topLeftX;
                Height = 1.0f;
                break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
                TopLeftX = topLeftX;
                TopLeftY = 0.0f;
                Width = (SubresourceWidth ? float(SubresourceWidth) : 1.0f) - topLeftX;
                Height = 1.0f;
                break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
                TopLeftX = topLeftX;
                TopLeftY = topLeftY;
                Width = (SubresourceWidth ? float(SubresourceWidth) : 1.0f) - topLeftX;
                Height = (SubresourceHeight ? float(SubresourceHeight) : 1.0f) - topLeftY;
                break;
            default: break;
            }

            MinDepth = minDepth;
            MaxDepth = maxDepth;
        }
    };
}

