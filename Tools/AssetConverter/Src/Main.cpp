#include "Ui/MainWindow.h"

#include "Os/WindowFactory.h"
#include "Crv/Renderers/HelloTriangleRenderer.h"
#include "Nmd/Logger.h"

#include "Crv/Renderers/ImguiRenderer.h"

#include <windows.h>
#include <iostream>

int main()
{
    const auto window = Os::WindowFactory::createWindow(Os::OsType::Windows);

    int width = 1000;
    int height = 750;

    if (!window->init("Papyrus: Asset Converter", width, height))
    {
        return 1;
    }

    auto windowHandler = static_cast<HWND>(window->getHandler());

    auto renderer = std::make_unique<Crv::ImguiRenderer>(windowHandler, width, height);
    renderer->init();

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
    window->setWindowResizable(false);
    window->show();

    Ui::MainWindow mainWindow{"Input", "Output"};

    Nmd::Logger::enableConsoleDebugging(false);
    Nmd::Logger::enableStreamLogging(true);

    while (!window->shouldClose())
    {
        window->processMessage();

        renderer->beginRender();

        mainWindow.render(width, height);

        renderer->endRender();
    }

    return 0;
}
