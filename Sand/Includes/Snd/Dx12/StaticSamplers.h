#pragma once

#include <vector>

namespace Snd::Dx12
{
    enum class ShaderVisibility;

    struct StaticSamplers
    {
    public:
        StaticSamplers() = default;

        void add(UINT shaderRegister, UINT registerSpace, ShaderVisibility shaderVisibility);
        [[nodiscard]] SIZE_T getSize() const { return m_sampler.size(); }
        [[nodiscard]] const std::vector<D3D12_STATIC_SAMPLER_DESC>& getDxSamplers() const { return m_sampler; }
    private:
        std::vector<D3D12_STATIC_SAMPLER_DESC> m_sampler;
    };
}
