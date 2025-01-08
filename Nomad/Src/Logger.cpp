#include "Nmd/Pch.h"
#include "Nmd/Logger.h"

#include <iostream>
#include <ostream>

namespace Nmd
{
    void Logger::log(const LogLevel level, const std::string &message)
    {
        const std::string logLevel = logLevelToString(level);
        std::cout << logLevel << " : " << message << std::endl;
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
