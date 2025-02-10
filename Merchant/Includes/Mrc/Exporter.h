#pragma once

#undef max // Undefine Windows' max macro
#include "Mrc/Data/AStaticModel.h"
#include <string>

namespace Mrc
{
    class Exporter
    {
    public:
        Exporter() = delete;
        static void exportAsStaticModel(const AStaticModel& staticModel, const std::string& exportDirectory, const std::string& name);
        static void exportAsClusteredStaticModel(const AStaticModel& staticModel, const std::string& exportDirectory, const std::string& name);
    };
}
