#include "Os/WindowFactory.h"
#include "Crv/Renderers/HelloTriangleRenderer.h"
#include "Nmd/Logger.h"

#include "Crv/Renderers/ImguiRenderer.h"

#include <imgui.h>
#include <windows.h>
#include <iostream>

#include "Ui/MainWindow.h"

int main()
{
    const auto window = Os::WindowFactory::createWindow(Os::OsType::Windows);

    if (!window->init("Papyrus", 800, 600))
    {
        return 1;
    }

    auto windowHandler = static_cast<HWND>(window->getHandler());

    auto renderer = std::make_unique<Crv::ImguiRenderer>(windowHandler, 800, 600);
    renderer->init();

    window->show();

    int width, height;

    // Window callbacks
    window->setMessageCallback([&renderer, &width, &height](const Os::Message& message) {
        switch (message.m_type)
        {
            case Os::MessageType::Resize:
                width = message.m_resize.m_width;
                height = message.m_resize.m_height;
                renderer->onResizeWindow(width, height);
                break;
            case Os::MessageType::Close:
                NOMAD_LOG(Nmd::LogLevel::Info, "Closing...");
                break;
            default:
                break;
        }
    });

    Ui::MainWindow mainWindow{"Input", "Output"};

    while (!window->shouldClose())
    {
        window->processMessage();

        renderer->beginRender();

        mainWindow.render(width, height);

        renderer->endRender();
    }

    return 0;
}
