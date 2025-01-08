#include "Snd/Dx12/StaticSamplers.h"
#include "Snd/Pch.h"

#include "Snd/Dx12/Utils/Common.h"

namespace Snd::Dx12
{
    void StaticSamplers::add(const UINT shaderRegister, const UINT registerSpace, ShaderVisibility shaderVisibility)
    {
        D3D12_STATIC_SAMPLER_DESC sampler = {};

        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = shaderRegister;                                                // Shader register (s0, s1, etc.)
        sampler.RegisterSpace = registerSpace;                                                  // Register space (default = 0)
        sampler.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderVisibility);      // Shader visibility (e.g., pixel shader)

        m_sampler.push_back(sampler);
    }
}
