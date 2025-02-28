#pragma once

#include "Arf/Data/StaticModel.h"
#include <Arf/Generated/StaticModel_generated.h>

namespace Mrc
{
    class StaticModelImporter
    {
    public:
        void import(const std::string &filePath, Arf::StaticModel& outModel);

    private:
        void importStaticModel(const std::string &filePath, Arf::StaticModel& outModel);
        void processModel(const Arf::Fbs::StaticModel* flatModel, Arf::StaticModel& outModel);
        void processMesh(const Arf::Fbs::StaticMesh* flatMesh, Arf::StaticMesh& outMesh);
    };
}
