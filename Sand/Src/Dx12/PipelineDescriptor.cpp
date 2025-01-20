#include "Snd/Pch.h"
#include "Snd/Dx12/PipelineDescriptor.h"

#include "Snd/Dx12/RootSignature.h"

#include <unordered_map>

namespace Snd::Dx12
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> ClassicGraphicsPipelineDescriptor::convertInputDescriptorToDx() const
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

        std::unordered_map<std::string, UINT> semanticIndexMap;

        for (const auto& attr : m_inputDescriptor)
        {
            D3D12_INPUT_ELEMENT_DESC elementDesc = {};

            elementDesc.SemanticName = attr.m_name.c_str();
            const UINT index = semanticIndexMap[attr.m_name]++;

            elementDesc.SemanticIndex = index;
            elementDesc.InputSlot = attr.m_binding;
            elementDesc.AlignedByteOffset = attr.m_offset;

            switch (attr.m_inputRate)
            {
            case InputRate::PerVertex:
                elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                elementDesc.InstanceDataStepRate = 0;
                break;
            case InputRate::PerInstance:
                elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                elementDesc.InstanceDataStepRate = 1;
                break;
            default:
                throw std::runtime_error("Unknown input rate ");
            }

            // Map InputAttribute::Type to DXGI_FORMAT
            switch (attr.m_type)
            {
            case InputParameterType::Float:
                elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
                break;
            case InputParameterType::Int:
                elementDesc.Format = DXGI_FORMAT_R32_SINT;
                break;
            case InputParameterType::Vector2:
                elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                break;
            case InputParameterType::Vector3:
                elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                break;
            case InputParameterType::Vector4:
                elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                break;
            default:
                throw std::runtime_error("Unknown vertex attribute type");
            }

            inputElementDescs.push_back(elementDesc);
        }

        return inputElementDescs;
    }

    D3D12_RASTERIZER_DESC ClassicGraphicsPipelineDescriptor::createDefaultRasterizerDescriptor()
    {
        D3D12_RASTERIZER_DESC desc;

        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        return desc;
    }

    D3D12_BLEND_DESC ClassicGraphicsPipelineDescriptor::createDefaultBlendDescriptor()
    {
        D3D12_BLEND_DESC desc;
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;

        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
        {
            FALSE,FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };

        for (auto & i : desc.RenderTarget)
        {
            i = defaultRenderTargetBlendDesc;
        }

        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC ClassicGraphicsPipelineDescriptor::createDefaultDepthStencilDescriptor()
    {
        // Define the default stencil operations
        constexpr D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
        {
            D3D12_STENCIL_OP_KEEP,        // StencilFailOp
            D3D12_STENCIL_OP_KEEP,        // StencilDepthFailOp
            D3D12_STENCIL_OP_KEEP,        // StencilPassOp
            D3D12_COMPARISON_FUNC_ALWAYS  // StencilFunc
        };

        // Initialize the descriptor
        D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};

        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

        depthStencilDesc.StencilEnable = FALSE;
        depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

        // Assign the same stencil operation settings to both front and back
        depthStencilDesc.FrontFace = defaultStencilOp;
        depthStencilDesc.BackFace  = defaultStencilOp;

        return depthStencilDesc;
    }
}