#pragma once
#include "Os/IWindow.h"

#include <windows.h>
#include <string>



namespace Os::Wnd
{

    class Window final : public IWindow
    {
    public:
        Window() : m_hwnd(nullptr), m_msg() {}

        bool init(const std::string &title, int width, int height) override;
        void show() override;
        bool shouldClose() override;
        void processMessage() override;

        void * getHandler() override;
    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static Key translateKey(WPARAM wParam);

    private:
        HWND m_hwnd;
        MSG m_msg;
    };
}