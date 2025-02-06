#pragma once

#include <memory>
#include <string>
#include <windows.h>

namespace Snd::Dx12
{
    class Context;
}

namespace Crv
{
    class ImguiRenderer
    {
    public:
        ImguiRenderer(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight);
        ~ImguiRenderer();

        ImguiRenderer(ImguiRenderer&& other) = delete;
        ImguiRenderer(const ImguiRenderer& other) = delete;
        ImguiRenderer& operator=(ImguiRenderer&& other) = delete;
        ImguiRenderer& operator=(const ImguiRenderer& other) = delete;

        void init();
        void onResizeWindow(UINT width, UINT height) const;

        void beginRender() const;
        void endRender() const;
    private:
        const HWND& m_hwnd;
        UINT m_displayWidth = 0;
        UINT m_displayHeight = 0;
        std::unique_ptr<Snd::Dx12::Context> m_context;
    };
}
