#pragma once

namespace Snd::Dx12
{
	struct ScissorRect : D3D12_RECT
	{
		ScissorRect() = default;
		explicit ScissorRect(const D3D12_RECT& o) noexcept :
			D3D12_RECT(o)
		{}
		explicit ScissorRect(
			const LONG Left,
			const LONG Top,
			const LONG Right,
			const LONG Bottom) noexcept
		{
			left = Left;
			top = Top;
			right = Right;
			bottom = Bottom;
		}
	};
}
