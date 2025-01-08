#pragma once

#include <codecvt>
#include <stdexcept>
#include <iostream>
#include <vector>

#include <Nmd/Logger.h>

#include "Nmd/Asserter.h"


namespace Snd::Dx12
{

    inline std::string HrToString(HRESULT hr)
    {
        char s_str[64] = {};
        sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
        return std::string(s_str);
    }

    class HrException : public std::runtime_error
    {
    public:
        HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
        HRESULT Error() const { return m_hr; }
    private:
        const HRESULT m_hr;
    };

#define SAFE_RELEASE(p) if (p) (p)->Release()

    inline void ThrowIfFailed(HRESULT hr)
    {
        NOMAD_ASSERT(Nmd::AssertType::Expect, !FAILED(hr), "Dx12 Error {}", HrToString(hr));
    }

    inline std::string wStringToString(const std::wstring& wstr)
    {
        // On Windows, use WideCharToMultiByte for UTF-8 conversion
        const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (bufferSize == 0) {
            NOMAD_LOG(Nmd::LogLevel::Error, "WideCharToMultiByte failed");
            throw std::runtime_error("Failed to convert wstring to string");
        }

        std::vector<char> buffer(bufferSize);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer.data(), bufferSize, nullptr, nullptr);
        return std::string(buffer.data());
    }


    inline std::wstring stringToWstr(const std::string& name)
    {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), (int)name.size(), NULL, 0);
        std::wstring wname(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, name.c_str(), (int)name.size(), &wname[0], sizeNeeded);

        return wname;
    }

    // Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
    inline void SetName(ID3D12Object* pObject, LPCWSTR name)
    {
        pObject->SetName(name);
    }
    inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
    {
        WCHAR fullName[50];
        if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
        {
            pObject->SetName(fullName);
        }
    }
#else
    inline void SetName(ID3D12Object*, LPCWSTR)
    {
    }
    inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
    {
    }
#endif

    // Naming helper for ComPtr<T>.
    // Assigns the name of the variable as the name of the object.
    // The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

    inline UINT CalculateConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffer size is required to be aligned.
        return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
    }

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
    inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target)
    {
        UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr;

        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errors;
        hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

        if (errors != nullptr)
        {
            OutputDebugStringA((char*)errors->GetBufferPointer());
        }
        ThrowIfFailed(hr);

        return byteCode;
    }
#endif

    // Resets all elements in a ComPtr array.
    template<class T>
    void ResetComPtrArray(T* comPtrArray)
    {
        for (auto& i : *comPtrArray)
        {
            i.Reset();
        }
    }


    // Resets all elements in a unique_ptr array.
    template<class T>
    void ResetUniquePtrArray(T* uniquePtrArray)
    {
        for (auto& i : *uniquePtrArray)
        {
            i.reset();
        }
    }

}
