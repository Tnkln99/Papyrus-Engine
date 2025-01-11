#pragma once

#include <memory>
#include <vector>

namespace Snd::Dx12
{
	class Texture2D;
	class Context;
	class PipelineState;
	class RootSignature;
	class Buffer;
	class DescriptorHeap;
}

namespace Crv
{
	class StaticMesh;

	class HelloTriangleRenderer
	{
	public:
		HelloTriangleRenderer(const HWND& hwnd, UINT displayWidth, UINT displayHeight);
		~HelloTriangleRenderer();

		void init();
		void onResizeWindow(UINT width, UINT height);

		// todo: update should not be here
		void update(float dt);
		void render() const;
	private:
		std::unique_ptr<Snd::Dx12::DescriptorHeap> m_inputDescriptorHeap{};

		struct SceneConstantBuffer
		{
			DirectX::XMFLOAT4 offset{};
			float padding[60]{}; // Padding so the constant buffer is 256-byte aligned.
		} m_constantBufferCpuData{};
		static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
		std::vector<std::shared_ptr<Snd::Dx12::Buffer>> m_constantBufferGpuData{};
		std::vector<UINT8*> m_constantBufferCpuVisibleGpuPointers;

		std::shared_ptr<Snd::Dx12::Texture2D> m_gpuTexture;
		std::vector<UINT8> m_cpuTextureData{};

		struct alignas(16) SquareVertex
		{
			DirectX::XMFLOAT4 m_position{};
			DirectX::XMFLOAT2 m_uv{};
		};
		const std::vector<uint16_t> m_indices =	{0, 1, 2, // First triangle (Top-left, Top-right, Bottom-right)
										0, 2, 3}; // Second triangle (Top-left, Bottom-right, Bottom-left);

		std::unique_ptr<StaticMesh> m_triangleMesh;

		std::unique_ptr<Snd::Dx12::PipelineState> m_helloTrianglePipeline;
		std::unique_ptr<Snd::Dx12::RootSignature> m_helloTriangleRootSignature;

		std::shared_ptr<Snd::Dx12::Context> m_context;

		const HWND& m_hwnd;
		UINT m_displayWidth;
		UINT m_displayHeight;
	};
}

