#include "Os/Pch.h"
#include "Os/Wnd/Window.h"

#include "Nmd/Logger.h"

#include "imgui.h"
#include <backends/imgui_impl_win32.h>


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Os::Wnd
{
    bool Window::init(const std::string &title, int width, int height)
    {
        WNDCLASS wc = {0};

        wc.lpfnWndProc   = Window::WindowProc;
        wc.hInstance     = GetModuleHandle(nullptr);
        wc.lpszClassName = reinterpret_cast<LPCSTR>(L"OasisWindowClass");

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            0, wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandle(nullptr), this);

        m_width = width;
        m_height = height;

        return (m_hwnd ? true : false);
    }

    void Window::show()
    {
        ShowWindow(m_hwnd, SW_SHOW);
    }

    bool Window::shouldClose()
    {
        return !GetMessage(&m_msg, nullptr, 0, 0);
    }

    void Window::processMessage()
    {
        TranslateMessage(&m_msg);
        DispatchMessage(&m_msg);
    }

    void* Window::getHandler()
    {
        return m_hwnd;
    }

    // Forward declare message handler from imgui_impl_win32.cpp

    LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        {
            return true;
        }

        Window* pThis = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            const auto* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = static_cast<Window*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (pThis)
        {
            switch (uMsg)
            {
                case WM_PAINT:
                {
                    if (pThis->m_messageCallback)
                    {
                        constexpr Message msg = { MessageType::Paint };
                        pThis->m_messageCallback(msg);
                    }
                    break;
                }
                case WM_SIZE:
                {
                    if (pThis->m_messageCallback)
                    {
                        RECT clientRect = {};
                        GetClientRect(hwnd, &clientRect);

                        Message msg = { MessageType::Resize };

                        msg.m_resize.m_width = clientRect.right - clientRect.left;
                        msg.m_resize.m_height = clientRect.bottom - clientRect.top;
                        pThis->m_messageCallback(msg);
                    }
                    break;
                }
                case WM_KEYDOWN:
                {
                    if (pThis->m_messageCallback)
                    {
                        Message msg = { MessageType::KeyDown };
                        const auto key = translateKey(static_cast<UINT8>(wParam));
                        msg.m_keyDownId = key;
                        pThis->m_messageCallback(msg);
                    }
                    break;
                }
                case WM_KEYUP:
                {
                    if (pThis->m_messageCallback)
                    {
                        Message msg = { MessageType::KeyUp };
                        const auto key = translateKey(static_cast<UINT8>(wParam));
                        msg.m_keyUpId = key;
                        pThis->m_messageCallback(msg);
                    }
                    break;
                }
                case WM_DESTROY:
                {
                    if (pThis->m_messageCallback)
                    {
                        constexpr Message msg = { MessageType::Close };
                        pThis->m_messageCallback(msg);
                    }
                    PostQuitMessage(0);
                    break;
                }
                default:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }
        return 0;
    }

    Key Window::translateKey(WPARAM wParam)
    {
        switch (wParam)
        {
            case 'W':
                return Key::ClvW;
            case 'A':
                return Key::ClvA;
            case 'S':
                return Key::ClvS;
            case 'D':
                return Key::ClvD;
            case VK_LEFT:
                return Key::ArwLeft;
            case VK_RIGHT:
                return Key::ArwRight;
            case VK_UP:
                return Key::ArwUp;
            case VK_DOWN:
                return Key::ArwDown;
            case VK_ESCAPE:
                return Key::ClvEsc;
            default:
                return Key::Unknown;
        }
    }
}

