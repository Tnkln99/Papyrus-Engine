#pragma once

#include "Snd/Dx12/Object.h"

#include <string>

namespace Snd::Dx12
{
	class RootSignature;
	class Device;

	struct ClassicGraphicsPipelineDescriptor;

	class PipelineState : public Object
	{
	public:
		PipelineState(const std::string& name, const Device& device, const ClassicGraphicsPipelineDescriptor& desc);
		~PipelineState();

		Microsoft::WRL::ComPtr<ID3D12PipelineState> getDxPipelineState() const;
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	};
}

