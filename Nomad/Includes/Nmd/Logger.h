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
        static std::string getLogString();
        static void clearLogString();

        static void enableConsoleDebugging(bool enable);
        static void enableStreamLogging(bool enable);
    private:
        static std::string logLevelToString(LogLevel level);

        static std::ostringstream s_logStream;

        static bool s_bEnableConsoleDebug;
        static bool s_bEnableStreamLogging;
    };
}

#define NOMAD_LOG(level, ...) Nmd::Logger::log(level, std::format(__VA_ARGS__))

