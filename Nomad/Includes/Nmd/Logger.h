#pragma once

#include <string>
#include <format>

namespace Nmd
{
    enum class LogLevel
    {
        Info,
        Message,
        Corruption,
        Warning,
        Error,
    };

    class Logger
    {
    public:
        Logger() = delete;
        static void log(LogLevel level, const std::string &message);
    private:
        static std::string logLevelToString(LogLevel level);
    };
}

#define NOMAD_LOG(level, ...) Nmd::Logger::log(level, std::format(__VA_ARGS__))

