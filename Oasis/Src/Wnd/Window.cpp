#include "Os/Pch.h"
#include "Os/Wnd/Window.h"

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

    LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
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
                        msg.m_keyId = static_cast<uint8_t>(wParam);

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
}

