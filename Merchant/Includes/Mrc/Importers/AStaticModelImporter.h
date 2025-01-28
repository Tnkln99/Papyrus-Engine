#pragma once

#include "IImporter.h"
#include <Mrc/Generated/AStaticModel_generated.h>

namespace Mrc
{
    class AStaticModelImporter final : public IImporter
    {
    public:
        void import(const std::string &filePath, AStaticModel& outScene) override;

    private:
        void importStaticModel(const std::string &filePath, AStaticModel& outScene);
        void processModel(const Mrc::StaticModel* flatModel, AStaticModel& outModel);
        void processMesh(const Mrc::StaticMesh* flatMesh, AStaticMesh& outMesh);
    };
}
