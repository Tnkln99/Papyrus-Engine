#pragma once
#include "IWindow.h"
#include "Os/OsType.h"

#include <memory>

namespace Os
{
    class WindowFactory
    {
    public:
        static std::unique_ptr<IWindow> createWindow(OsType type);
    };
}