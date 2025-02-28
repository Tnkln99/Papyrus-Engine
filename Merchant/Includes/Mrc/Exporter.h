#pragma once

#include "Arf/Data/StaticModel.h"
#include <string>

namespace Mrc
{
    struct ProcessOptions;

    class Exporter
    {
    public:
        Exporter() = delete;
        static void exportAsStaticModel(const Arf::StaticModel& staticModel, const std::string& exportDirectory, const std::string& name);
    };
}
