#pragma once

#include "Resource.h"

#include <Nmd/Asserter.h>

namespace Snd::Dx12
{
	class Buffer;
	class Swapchain;
	class Device;
	class DescriptorHeap;

	enum class TextureDimension
	{
		Texture1D,
		Texture2D,
		TextureArray2D,
		Texture3D,
	};

	enum class TextureFormat
	{
		R8G8B8A8Unorm =		DXGI_FORMAT_R8G8B8A8_UNORM,
		B8G8R8A8Unorm =		DXGI_FORMAT_B8G8R8A8_UNORM,
		R8G8B8A8UnormSrgb = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		B8G8R8A8UnormSrgb = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		R16G16B16A16Float = DXGI_FORMAT_R16G16B16A16_FLOAT,
		R32G32B32A32Float = DXGI_FORMAT_R32G32B32A32_FLOAT,
		D32Float =			DXGI_FORMAT_D32_FLOAT,
		D24UnormS8Uint =	DXGI_FORMAT_D24_UNORM_S8_UINT
	};

	enum class TextureLayout
	{
		Unknown =				D3D12_TEXTURE_LAYOUT_UNKNOWN,
		RowMajor =				D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		UndefinedSwizzle64KB =	D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE,
		StandardSwizzle64KB =	D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE,
	};

	class Texture2D final : public Resource
	{
	public:
		Texture2D(
			const std::string& name,
			const Device& device,
			ResourceHeapType heapType,
			ResourceFlag::Type flags,
			UINT width,
			UINT height,
			TextureFormat format,
			ResourceState initialState = ResourceState::GenericRead,
			UINT16 mipLevels = 1,
			UINT16 arraySize = 1,
			UINT sampleCount = 1
		);

		~Texture2D() override = default;

		[[nodiscard]] UINT getWidth() const { return m_width; }
		[[nodiscard]] UINT getHeight() const { return m_height; }
		[[nodiscard]] TextureFormat getFormat() const { return m_format; }
		[[nodiscard]] UINT getFormatByteSize() const { return getFormatByteSize(m_format); }
		[[nodiscard]] UINT16 getMipLevels() const { return m_mipLevels; }
		[[nodiscard]] UINT16 getArraySize() const { return m_arraySize; }
		[[nodiscard]] UINT getSampleCount() const { return m_sampleCount; }

		[[nodiscard]] UINT64 getRequiredIntermediateSize(UINT firstSubResource, UINT numSubResources) const;

	private:
		UINT m_width;
		UINT m_height;
		TextureFormat m_format;
		UINT16 m_mipLevels;
		UINT16 m_arraySize;
		UINT m_sampleCount;

		static UINT getFormatByteSize(TextureFormat format);
	};

	// this texture is controlled by Swapchain itself
	class SwapchainRenderTarget final : public Resource
	{
	public:
		SwapchainRenderTarget(const Device& device, const DescriptorHeap& rtvHeap, Swapchain& swapchain, UINT bufferIndex);
	private:
		Swapchain& m_swapchain;
	};
}

