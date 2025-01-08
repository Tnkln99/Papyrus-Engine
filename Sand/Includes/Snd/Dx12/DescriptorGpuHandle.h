#pragma once

namespace Snd::Dx12
{
	struct DescriptorGpuHandle : public D3D12_GPU_DESCRIPTOR_HANDLE
	{
        DescriptorGpuHandle() = default;
        explicit DescriptorGpuHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& o) noexcept :
            D3D12_GPU_DESCRIPTOR_HANDLE(o)
        {}
        DescriptorGpuHandle(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other, INT offsetScaledByIncrementSize) noexcept
        {
            InitOffsetted(other, offsetScaledByIncrementSize);
        }
        DescriptorGpuHandle(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other, INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
        }
        DescriptorGpuHandle& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            ptr = UINT64(INT64(ptr) + INT64(offsetInDescriptors) * INT64(descriptorIncrementSize));
            return *this;
        }
        DescriptorGpuHandle& Offset(INT offsetScaledByIncrementSize) noexcept
        {
            ptr = UINT64(INT64(ptr) + INT64(offsetScaledByIncrementSize));
            return *this;
        }
        inline bool operator==(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other) const noexcept
        {
            return (ptr == other.ptr);
        }
        inline bool operator!=(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other) const noexcept
        {
            return (ptr != other.ptr);
        }
        DescriptorGpuHandle& operator=(const D3D12_GPU_DESCRIPTOR_HANDLE& other) noexcept
        {
            ptr = other.ptr;
            return *this;
        }

        inline void InitOffsetted(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize) noexcept
        {
            InitOffsetted(*this, base, offsetScaledByIncrementSize);
        }

        inline void InitOffsetted(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
        }

        static inline void InitOffsetted(_Out_ D3D12_GPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize) noexcept
        {
            handle.ptr = UINT64(INT64(base.ptr) + INT64(offsetScaledByIncrementSize));
        }

        static inline void InitOffsetted(_Out_ D3D12_GPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            handle.ptr = UINT64(INT64(base.ptr) + INT64(offsetInDescriptors) * INT64(descriptorIncrementSize));
        }
	};
}