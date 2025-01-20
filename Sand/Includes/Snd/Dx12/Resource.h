#pragma once

#include <string>

#include "Object.h"

namespace Snd::Dx12
{
	class GraphicsCommandList;

	struct ResourceFlag
	{
		enum Type : unsigned int
		{
			None =					D3D12_RESOURCE_FLAG_NONE,
			AllowRenderTarget =		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			AllowDepthStencil =		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			AllowUnorderedAccess =	D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			AllowShaderResource =	D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
		};
	};

	struct HeapFlag
	{
		enum Type : unsigned int
		{
			None =	D3D12_HEAP_FLAG_NONE
		};
	};

	enum class ResourceHeapType
	{
		Default =	D3D12_HEAP_TYPE_DEFAULT,
		Upload =	D3D12_HEAP_TYPE_UPLOAD,
	};

	enum class ResourceState
	{
		Common =					D3D12_RESOURCE_STATE_COMMON,
		RenderTarget =				D3D12_RESOURCE_STATE_RENDER_TARGET,
		Present =					D3D12_RESOURCE_STATE_PRESENT,
		VertexAndConstantBuffer =	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		CopyDest =					D3D12_RESOURCE_STATE_COPY_DEST,
		CopySource =				D3D12_RESOURCE_STATE_COPY_SOURCE,
		GenericRead =				D3D12_RESOURCE_STATE_GENERIC_READ,
		PixelShaderResource =		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		DepthStencilResource =		D3D12_RESOURCE_STATE_DEPTH_WRITE,
	};

	enum class ResourceBarrierType
	{
		Transition =	D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		Aliasing =		D3D12_RESOURCE_BARRIER_TYPE_ALIASING,
		Uav =			D3D12_RESOURCE_BARRIER_TYPE_UAV
	};

	struct ResourceBarrierFlag
	{
		enum Type : unsigned int
		{
			None =	D3D12_RESOURCE_BARRIER_FLAG_NONE,
			Begin = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY,
			End =	D3D12_RESOURCE_BARRIER_FLAG_END_ONLY,
		};
	};

	class Resource : public Object
	{
	public:
		virtual ~Resource() = default;
	protected:
		Resource() = default;
		Resource(const std::string& name, ResourceState state, ResourceHeapType heaptype, ResourceFlag::Type flags, HeapFlag::Type heapFlag = HeapFlag::None);

	public:
		void transition(
			const GraphicsCommandList& commandList,
			ResourceState afterState,
			UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			ResourceBarrierFlag::Type flags = ResourceBarrierFlag::None,
			ResourceBarrierType type = ResourceBarrierType::Transition);

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> getDxResource() const;
		[[nodiscard]] UINT64 getGpuVirtualAddress() const;
		[[nodiscard]] std::string getName() const{ return m_name; }
		[[nodiscard]] ResourceState getState() const { return m_state; }

		void map(SIZE_T begin, SIZE_T end, void **ppData) const;
		void unmap() const;
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;

		ResourceHeapType m_heapType;
		ResourceState m_state;
		ResourceFlag::Type m_flags;
		HeapFlag::Type m_heapFlags;
	};
}
