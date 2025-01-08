#include "Snd/Pch.h"
#include "Snd/Dx12/RootSignature.h"

#include "Snd/Dx12/Device.h"
#include "Snd/Dx12/Utils/DxHelpers.h"
#include "Snd/Dx12/StaticSamplers.h"
#include "Snd/Dx12/Utils/Common.h"

namespace Snd::Dx12
{
#pragma region DescriptorRange

    D3D12_DESCRIPTOR_RANGE DescriptorTableParameter::getDxRange() const
    {
        D3D12_DESCRIPTOR_RANGE range = {};
        range.RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(rangeType);
        range.NumDescriptors = numDescriptors;
        range.BaseShaderRegister = baseShaderRegister;
        range.RegisterSpace = registerSpace;
        range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
        return range;
    }
#pragma endregion

#pragma region RootParameter
    D3D12_ROOT_PARAMETER RootParameter::getDxRootParameter() const
    {
        D3D12_ROOT_PARAMETER param = {};
        param.ParameterType = static_cast<D3D12_ROOT_PARAMETER_TYPE>(m_parameterType);
        param.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(m_visibility);

        switch (m_parameterType)
        {
            case RootParameterType::Table:
            {
                // Convert DescriptorRanges to D3D12_DESCRIPTOR_RANGE array
                std::vector<D3D12_DESCRIPTOR_RANGE> ranges(m_tableParameters.size());
                for (size_t i = 0; i < m_tableParameters.size(); ++i)
                {
                    ranges[i] = m_tableParameters[i].getDxRange();
                }

                // Allocate memory for ranges (ensure lifetime)
                descriptorTableRanges = std::move(ranges);

                param.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorTableRanges.size());
                param.DescriptorTable.pDescriptorRanges = descriptorTableRanges.data();
                break;
            }
            case RootParameterType::Constants32Bit:
            {
                param.Constants.ShaderRegister = m_shaderRegister;
                param.Constants.RegisterSpace = m_registerSpace;
                param.Constants.Num32BitValues = m_constant32BitValues;
                break;
            }
            case RootParameterType::Cbv:
            case RootParameterType::Uav:
            case RootParameterType::Srv:
            {
                param.Descriptor.ShaderRegister = m_shaderRegister;
                param.Descriptor.RegisterSpace = m_registerSpace;
                break;
            }
            default:
                NOMAD_ASSERT(Nmd::AssertType::Assert, true, "Unknown Root Parameter Type");
        }

        return param;
    }
#pragma endregion

#pragma region RootSignature

    RootSignature::RootSignature(const std::string& name, const Device& device,
        const StaticSamplers &samplers,
        const std::vector<RootParameter>& parameters,
        RootSignatureFlag::Type flags) : Object(name)
    {
        std::vector<D3D12_ROOT_PARAMETER> d3dRootParameters(parameters.size());
        for (size_t i = 0; i < parameters.size(); ++i)
        {
            d3dRootParameters[i] = parameters[i].getDxRootParameter();
        }


        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters = static_cast<UINT>(d3dRootParameters.size());
        rootSignatureDesc.pParameters = d3dRootParameters.data();
        rootSignatureDesc.NumStaticSamplers = static_cast<UINT>(samplers.getSize());
        rootSignatureDesc.pStaticSamplers = samplers.getDxSamplers().data();
        rootSignatureDesc.Flags = static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(flags);

        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob));

        ThrowIfFailed(device.getDxDevice()->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&m_rootSignature)));

        ThrowIfFailed(m_rootSignature->SetName(stringToWstr(m_name).c_str()));
    }

#pragma endregion
}
