#include "Nmd/Pch.h"
#include "Nmd/Logger.h"

#include <iostream>
#include <ostream>
#include <sstream>

namespace Nmd
{
    std::ostringstream Logger::s_logStream;
    bool Logger::s_bEnableConsoleDebug = true;
    bool Logger::s_bEnableStreamLogging = false;

    void Logger::log(const LogLevel level, const std::string &message)
    {
        const std::string logLevel = logLevelToString(level);
        if (s_bEnableConsoleDebug)
        {
            std::cout << logLevel << " : " << message << std::endl;
        }

        if (s_bEnableStreamLogging)
        {
            s_logStream << logLevel << " : " << message << std::endl;
        }
    }

    std::string Logger::getLogString()
    {
        // Return a copy of the accumulated logs
        return s_logStream.str();
    }

    void Logger::clearLogString()
    {
        // Clear the internal stream
        s_logStream.str("");
        s_logStream.clear();
    }

    void Logger::enableConsoleDebugging(const bool enable)
    {
        s_bEnableConsoleDebug = enable;
    }

    void Logger::enableStreamLogging(const bool enable)
    {
        s_bEnableStreamLogging = enable;
    }

    std::string Logger::logLevelToString(const LogLevel level)
    {
        std::string logLevel;
        switch (level)
        {
            case LogLevel::Info:
            {
                logLevel = "Info";
                break;
            }
            case LogLevel::Warning:
            {
                logLevel = "Warning";
                break;
            }
            case LogLevel::Error:
            {
                logLevel = "Error";
                break;
            }
            case LogLevel::Corruption:
            {
                logLevel = "Corruption";
                break;
            }
            default: ;
        }
        return logLevel;
    }
}
