#pragma once

#include "Snd/Dx12/Object.h"

#include <Nmd/Asserter.h>
#include <vector>

namespace Snd::Dx12
{
    class Device;

    enum class DescriptorTableParameterType
    {
        Srv = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        Uav = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
        Cbv = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,

        // not supported yet
        // Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
    };

    class DescriptorTableParameter
    {
    public:
        DescriptorTableParameter(const DescriptorTableParameterType type, const UINT numDescriptors,
            const UINT baseShaderRegister, const UINT registerSpace = 0,
            const UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
            : rangeType(type),
            numDescriptors(numDescriptors),
            baseShaderRegister(baseShaderRegister),
            registerSpace(registerSpace),
            offsetInDescriptorsFromTableStart(offsetInDescriptorsFromTableStart)
        {}

        // Method to convert to D3D12_DESCRIPTOR_RANGE
        [[nodiscard]] D3D12_DESCRIPTOR_RANGE getDxRange() const;
    private:
        DescriptorTableParameterType rangeType;
        UINT numDescriptors;
        UINT baseShaderRegister;
        UINT registerSpace;
        UINT offsetInDescriptorsFromTableStart;
    };

    enum class RootParameterType
    {
        Table = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,

        Constants32Bit = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
        Srv = D3D12_ROOT_PARAMETER_TYPE_SRV,
        Cbv = D3D12_ROOT_PARAMETER_TYPE_CBV,
        Uav = D3D12_ROOT_PARAMETER_TYPE_UAV,
    };

    enum class ShaderVisibility;

    class RootParameter
    {
    public:
        // Constructor for non-table types
        RootParameter(const RootParameterType type, const ShaderVisibility shaderVisibility,
            const UINT shaderRegister, const UINT registerSpace, const UINT constant32BitValues = 0)
            : m_parameterType(type),
            m_visibility(shaderVisibility),
            m_shaderRegister(shaderRegister),
            m_registerSpace(registerSpace),
            m_constant32BitValues(constant32BitValues)
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, type != RootParameterType::Table, "If you want to declare a table type use other constructor who is specific for Table types");
        }

        // Constructor for descriptor tables
        RootParameter(const RootParameterType type, const ShaderVisibility shaderVisibility, const std::vector<DescriptorTableParameter>& tableParameters)
            : m_parameterType(type),
            m_visibility(shaderVisibility),
            m_tableParameters(tableParameters)
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, type == RootParameterType::Table, "This constructor is only for table parameter types");
        }

        // Method to convert to D3D12_ROOT_PARAMETER
        D3D12_ROOT_PARAMETER getDxRootParameter() const;

        // For non-table types
        RootParameterType m_parameterType;
        ShaderVisibility m_visibility;
        UINT m_shaderRegister = 0;
        UINT m_registerSpace = 0;

        // For 32-bit constants
        UINT m_constant32BitValues = 0;

        // For descriptor tables
        std::vector<DescriptorTableParameter> m_tableParameters;

    private:
        // To keep the ranges alive for the duration of root signature creation
        mutable std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableRanges;
    };

    struct RootSignatureFlag
    {
        enum Type : unsigned int
        {
            None = D3D12_ROOT_SIGNATURE_FLAG_NONE,
            AllowInputAssembler = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
        };
    };

    struct StaticSamplers;

	class RootSignature : public Object
	{
	public:
        explicit RootSignature(const std::string& name, const Device& device,
            const StaticSamplers &samplers,
            const std::vector<RootParameter>& parameters = std::vector<RootParameter>{},
            RootSignatureFlag::Type flags = RootSignatureFlag::None);

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12RootSignature> getDxRootSignature() const
        {
            return m_rootSignature;
        }
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}

