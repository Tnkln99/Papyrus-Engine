#include "Crv/Renderers/ImguiRenderer.h"
#include "Snd/Dx12/Context.h"

namespace Crv
{
    ImguiRenderer::ImguiRenderer(const HWND &hwnd, uint32_t displayWidth, uint32_t displayHeight) :  m_hwnd(hwnd), m_displayWidth(displayWidth), m_displayHeight(displayHeight)
    {
    }

    ImguiRenderer::~ImguiRenderer()
    {
        m_context->cleanUpImgui();
    }

    void ImguiRenderer::init()
    {
        m_context = std::make_unique<Snd::Dx12::Context>(m_hwnd, m_displayWidth, m_displayHeight);
        m_context->initImgui(m_hwnd);
        m_context->init();
    }

    void ImguiRenderer::onResizeWindow(const UINT width, const UINT height) const
    {
        m_context->onResizeWindow(width, height);
    }

    void ImguiRenderer::beginRender() const
    {
        m_context->newFrameImgui();

        m_context->preRecording();
        m_context->clear();
    }

    void ImguiRenderer::endRender() const
    {
        m_context->renderImgui();

        m_context->postRecording();
        m_context->flush();
        m_context->moveToNextFrame();
    }
}
