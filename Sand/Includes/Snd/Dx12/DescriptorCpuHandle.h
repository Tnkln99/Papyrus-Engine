#pragma once

namespace Snd::Dx12
{
	struct DescriptorCpuHandle : public D3D12_CPU_DESCRIPTOR_HANDLE
	{
        DescriptorCpuHandle() = default;
        explicit DescriptorCpuHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& o) noexcept :
            D3D12_CPU_DESCRIPTOR_HANDLE(o)
        {}
        DescriptorCpuHandle(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other, INT offsetScaledByIncrementSize) noexcept
        {
            InitOffsetted(other, offsetScaledByIncrementSize);
        }
        DescriptorCpuHandle(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other, INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
        }
        DescriptorCpuHandle& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            ptr = SIZE_T(INT64(ptr) + INT64(offsetInDescriptors) * INT64(descriptorIncrementSize));
            return *this;
        }
        DescriptorCpuHandle& Offset(INT offsetScaledByIncrementSize) noexcept
        {
            ptr = SIZE_T(INT64(ptr) + INT64(offsetScaledByIncrementSize));
            return *this;
        }
        bool operator==(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other) const noexcept
        {
            return (ptr == other.ptr);
        }
        bool operator!=(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other) const noexcept
        {
            return (ptr != other.ptr);
        }
        DescriptorCpuHandle& operator=(const D3D12_CPU_DESCRIPTOR_HANDLE& other) noexcept
        {
            ptr = other.ptr;
            return *this;
        }

        inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize) noexcept
        {
            InitOffsetted(*this, base, offsetScaledByIncrementSize);
        }

        inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
        }

        static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize) noexcept
        {
            handle.ptr = SIZE_T(INT64(base.ptr) + INT64(offsetScaledByIncrementSize));
        }

        static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept
        {
            handle.ptr = SIZE_T(INT64(base.ptr) + INT64(offsetInDescriptors) * INT64(descriptorIncrementSize));
        }
	};
}


