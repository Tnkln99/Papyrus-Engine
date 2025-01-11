#include "Os/WindowFactory.h"
#include "Crv/Renderers/HelloTriangleRenderer.h"
#include "Nmd/Logger.h"
#include "Mrc/Importer.h"

#include <windows.h>
#include <iostream>

int main()
{
    const auto window = Os::WindowFactory::createWindow(Os::OsType::Windows);

    if (!window->init("Papyrus", 800, 600))
    {
        return 1;
    }

    auto windowHandler = static_cast<HWND>(window->getHandler());

    auto renderer = std::make_unique<Crv::HelloTriangleRenderer>(windowHandler, 800, 600);
    renderer->init();

    window->show();

    // Window callbacks
    window->setMessageCallback([&renderer](const Os::Message& message) {
        switch (message.m_type)
        {
            case Os::MessageType::Paint:
                renderer->update(1); // todo: calculate dt
                renderer->render();
                break;

            case Os::MessageType::Resize:
                renderer->onResizeWindow(message.m_resize.m_width, message.m_resize.m_height);
                break;
            case Os::MessageType::Close:
                NOMAD_LOG(Nmd::LogLevel::Info, "Closing...");
                break;
            default:
                break;
        }
    });

    while (!window->shouldClose())
    {
        window->processMessage();
    }

    return 0;
}
