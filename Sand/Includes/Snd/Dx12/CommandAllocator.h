#pragma once

#include "Snd/Dx12/Object.h"

namespace Snd::Dx12
{
	class Device;
	enum class GraphicsCommandListType;

	class CommandAllocator : public Object
	{
	public:
		CommandAllocator(const std::string &name, const Device& device, GraphicsCommandListType commandListType);

		[[nodiscard]] GraphicsCommandListType getType() const
		{
			return m_type;
		}

		void reset() const;

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandAllocator> getDxAllocator() const;
	private:
		GraphicsCommandListType m_type;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	};
}


