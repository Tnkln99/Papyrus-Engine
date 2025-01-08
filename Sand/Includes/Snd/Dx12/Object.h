#pragma once

#include <string>
#include <utility>

namespace Snd::Dx12
{
    // parent of all dx12 api objects
    class Object
    {
    protected:
        explicit Object(std::string name) : m_name(std::move(name)) {}
        Object() = default;

    protected:
        std::string m_name;
    };
}
