#pragma once

#include "Mrc/Data/AStaticModel.h"

namespace Mrc
{
    class AClusteredStaticModelExporter
    {
    public:
        void exportAsClusteredStaticModel(const AStaticModel& staticModel, const std::string &dirName, const std::string &fileName);
    private:

    };
}
