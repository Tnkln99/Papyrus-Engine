#include "Os/WindowFactory.h"
#include "Os/Pch.h"

#include "Os/Wnd/Window.h"

#include "Nmd/Logger.h"

#include <stdexcept>

namespace Os
{
    std::unique_ptr<IWindow> WindowFactory::createWindow(const OsType type)
    {
        switch (type)
        {
            case OsType::Windows:
                return std::make_unique<Wnd::Window>();
            default:
                NOMAD_LOG(Nmd::LogLevel::Error, "Unknown Window type");
                return nullptr;
        }
    }
}
