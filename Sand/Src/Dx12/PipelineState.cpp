#include "Snd/Pch.h"
#include "Snd/Dx12/PipelineState.h"

#include <stdexcept>

#include "Snd/Dx12/Utils/DxHelpers.h"

#include "Snd/Dx12/RootSignature.h"
#include "Snd/Dx12/PipelineDescriptor.h"
#include "Snd/Dx12/Device.h"

namespace Snd::Dx12
{
    PipelineState::PipelineState(const std::string& name, const Device& device, const ClassicGraphicsPipelineDescriptor& desc) : Object(name)
    {
        // Convert the input descriptor to D3D12_INPUT_ELEMENT_DESC
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescriptors = desc.convertInputDescriptorToDx();

        // Fill out the D3D12_GRAPHICS_PIPELINE_STATE_DESC structure
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

        psoDesc.InputLayout = { inputElementDescriptors.data(), static_cast<UINT>(inputElementDescriptors.size()) };
        psoDesc.pRootSignature = desc.m_rootSignature.getDxRootSignature().Get();

        // Set shader bytecodes
        psoDesc.VS = { desc.m_vertexShader.m_data, desc.m_vertexShader.m_size };
        psoDesc.PS = { desc.m_pixelShader.m_data, desc.m_pixelShader.m_size };

        // Set up default rasterizer, blend, and depth-stencil states
        psoDesc.RasterizerState = ClassicGraphicsPipelineDescriptor::createDefaultRasterizerDescriptor();
        psoDesc.BlendState = ClassicGraphicsPipelineDescriptor::createDefaultBlendDescriptor();
        psoDesc.DepthStencilState = ClassicGraphicsPipelineDescriptor::createDefaultDepthStencilDescriptor();

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        // Assuming a single render target of format DXGI_FORMAT_R8G8B8A8_UNORM
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        // Set the sample description
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;

        ThrowIfFailed(device.getDxDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        ThrowIfFailed(m_pipelineState->SetName(stringToWstr(m_name).c_str()));
    }

    PipelineState::~PipelineState()
    {
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState::getDxPipelineState() const
    {
        return m_pipelineState;
    }
}
