#pragma once

#include <string>
#include <format>

namespace Nmd
{
    enum class AssertType
    {
        Assert,
        Expect
    };

    class Asserter
    {
    public:
        Asserter() = delete;

        static void assertation(AssertType type, bool condition, const std::string &message);
    };

    #define NOMAD_ASSERT(assertType, condition, ...) \
        Nmd::Asserter::assertation(assertType, condition, std::format("File: {}, Line: {}, Function: {}: {}", __FILE__, __LINE__, __func__, std::format(__VA_ARGS__)))
}
