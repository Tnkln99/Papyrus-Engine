#include "Snd/Pch.h"
#include "Snd/Dx12/Device.h"

#include "Snd/Dx12/Utils/DxHelpers.h"

#include <Nmd/Logger.h>

#include <cassert>
#include <dxgidebug.h>
#include <sstream>

using Microsoft::WRL::ComPtr;

namespace Snd::Dx12
{
    Device::Device(const std::string& name) : Object(name)
	{
        UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
        // Enable the debug layer (requires the Graphics Tools "optional feature").
        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();

                Microsoft::WRL::ComPtr<ID3D12Debug1> debugController1;
                if (SUCCEEDED((debugController->QueryInterface(IID_PPV_ARGS(&debugController1)))))
                {
                    debugController1->SetEnableGPUBasedValidation(true);
                }

                // Enable additional debug layers.
                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif
        ComPtr<IDXGIFactory4> factory;

        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

        ComPtr<IDXGIAdapter1> hardwareAdapter;
        getHardwareAdapter(factory.Get(), &hardwareAdapter);


        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_12_2,
            IID_PPV_ARGS(&m_device)
        ));

        m_adaptor = hardwareAdapter;

        // get all the information necessary and and print it
        DXGI_ADAPTER_DESC desc;
        ThrowIfFailed(m_adaptor->GetDesc(&desc));

        printAdaptorDesc(desc);
#if defined(_DEBUG)
        enableDebugMessages();
#endif

        ThrowIfFailed(m_device->SetName(stringToWstr(m_name).c_str()));
	}

    ComPtr<ID3D12Device> Device::getDxDevice() const
    {
        return m_device;
    }

    void Device::enableDebugMessages() const
    {
        ComPtr<ID3D12InfoQueue1> pInfoQueue = nullptr;
        ThrowIfFailed(m_device->QueryInterface(IID_PPV_ARGS(&pInfoQueue)));

        DWORD callbackCookie;
        pInfoQueue->RegisterMessageCallback(
            debugCallback,
            D3D12_MESSAGE_CALLBACK_FLAG_NONE,
            nullptr,
            &callbackCookie);
    }

    void Device::printAdaptorDesc(DXGI_ADAPTER_DESC desc) const
    {
        ThrowIfFailed(m_adaptor->GetDesc(&desc));
        NOMAD_LOG(Nmd::LogLevel::Info, "Selected GPU : {}", wStringToString(desc.Description));
    }

    void Device::debugCallback(
        const D3D12_MESSAGE_CATEGORY category,
        const D3D12_MESSAGE_SEVERITY severity,
        const D3D12_MESSAGE_ID id,
        const LPCSTR pDescription,
        void* pContext)
    {
	    auto categoryStr = "";
        switch (category)
        {
        case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED: categoryStr = "Application Defined"; break;
        case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS: categoryStr = "Miscellaneous"; break;
        case D3D12_MESSAGE_CATEGORY_INITIALIZATION: categoryStr = "Initialization"; break;
        case D3D12_MESSAGE_CATEGORY_CLEANUP: categoryStr = "Cleanup"; break;
        case D3D12_MESSAGE_CATEGORY_COMPILATION: categoryStr = "Compilation"; break;
        case D3D12_MESSAGE_CATEGORY_STATE_CREATION: categoryStr = "State Creation"; break;
        case D3D12_MESSAGE_CATEGORY_STATE_SETTING: categoryStr = "State Setting"; break;
        case D3D12_MESSAGE_CATEGORY_STATE_GETTING: categoryStr = "State Getting"; break;
        case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION: categoryStr = "Resource Manipulation"; break;
        case D3D12_MESSAGE_CATEGORY_EXECUTION: categoryStr = "Execution"; break;
        case D3D12_MESSAGE_CATEGORY_SHADER: categoryStr = "Shader"; break;
        }

        Nmd::LogLevel level;
        switch (severity)
        {
            case D3D12_MESSAGE_SEVERITY_CORRUPTION: level = Nmd::LogLevel::Corruption; break;
            case D3D12_MESSAGE_SEVERITY_ERROR: level = Nmd::LogLevel::Error; break;
            case D3D12_MESSAGE_SEVERITY_WARNING: level = Nmd::LogLevel::Warning; break;
            case D3D12_MESSAGE_SEVERITY_INFO: level = Nmd::LogLevel::Info; break;
            case D3D12_MESSAGE_SEVERITY_MESSAGE: level = Nmd::LogLevel::Corruption; break;
            default: level = Nmd::LogLevel::Info; break;
        }

        NOMAD_LOG(level, " {}"     "\n"
            "Message id: {}         \n"
            "Category: {}           \n"
            ,pDescription, std::to_string(id), categoryStr);

    }

    _Use_decl_annotations_
    void Device::getHardwareAdapter(
            IDXGIFactory1* pFactory,
            IDXGIAdapter1** ppAdapter,
            bool requestHighPerformanceAdapter)
    {
        *ppAdapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;

        ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (
                UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                    IID_PPV_ARGS(&adapter)));
                    ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                ThrowIfFailed(adapter->GetDesc1(&desc));

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        if (adapter.Get() == nullptr)
        {
            for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                ThrowIfFailed(adapter->GetDesc1(&desc));

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }
}
