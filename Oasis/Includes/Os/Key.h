#pragma once

#include <string>

namespace Os
{
    enum class Key
    {
        Unknown,
        ArwUp,
        ArwDown,
        ArwLeft,
        ArwRight,
        ClvW,
        ClvA,
        ClvS,
        ClvD,
        ClvEsc,
    };

    inline std::string toString(Key key)
    {
        switch (key)
        {
            case Key::ArwUp:
                return "ArwUp";
            case Key::ArwDown:
                return "ArwDown";
            case Key::ArwLeft:
                return "ArwLeft";
            case Key::ArwRight:
                return "ArwRight";
            case Key::ClvW:
                return "ClvW";
            case Key::ClvA:
                return "ClvA";
            case Key::ClvS:
                return "ClvS";
            case Key::ClvD:
                return "ClvD";
            case Key::ClvEsc:
                return "ClvEsc";
            default:
                return "Unknown";
        }
    }
}
