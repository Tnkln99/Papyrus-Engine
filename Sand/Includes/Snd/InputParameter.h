#pragma once

#include <utility>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace Snd
{
    enum class InputParameterType
    {
        Float,
        Int,
        Vector2,
        Vector3,
        Vector4,
    };

    enum class InputRate
    {
        PerVertex,
        PerInstance
    };

    // Input assembly parameter
    struct InputParameter
    {
        // Input parameter for input assembly
        InputParameter(std::string name, const InputParameterType type, const uint32_t offset, const size_t binding, const InputRate inputRate)
            : m_name {std::move(name)}, m_type {type}, m_offset{offset}, m_binding {binding}, m_inputRate {inputRate}
        {}

        std::string m_name;
        InputParameterType m_type;
        uint32_t m_offset;
        size_t m_binding;
        InputRate m_inputRate;
    };
}
