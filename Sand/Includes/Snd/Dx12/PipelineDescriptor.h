#pragma once

#include "Snd/ShaderCode.h"
#include "Snd/InputParameter.h"

#include <vector>

namespace Snd::Dx12
{
	class RootSignature;

	struct ClassicGraphicsPipelineDescriptor
	{
		RootSignature& m_rootSignature;
		ShaderCode m_vertexShader;
		ShaderCode m_pixelShader;
		std::vector<InputParameter> m_inputDescriptor;

		[[nodiscard]] std::vector<D3D12_INPUT_ELEMENT_DESC> convertInputDescriptorToDx() const;
		static D3D12_RASTERIZER_DESC createDefaultRasterizerDescriptor();
		static D3D12_BLEND_DESC createDefaultBlendDescriptor();
		static D3D12_DEPTH_STENCIL_DESC createDefaultDepthStencilDescriptor();
	};

	class ComputePipelineDescriptor
	{
		// TODO 
	};
}

