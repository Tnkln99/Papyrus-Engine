#pragma once

#include "IImporter.h"
#include <Mrc/Generated/AScene_generated.h>

namespace Mrc
{
    class ASceneImporter final : public IImporter
    {
    public:
        void import(const std::string &filePath, AScene& outScene) override;

    private:
        void importScene(const std::string &filePath, AScene& outScene);
        void processScene(const Mrc::Scene* flatScene, AScene& outScene);
        void processModel(const Mrc::StaticModel* flatModel, AStaticModel& outModel);
        void processMesh(const Mrc::StaticMesh* flatMesh, AStaticMesh& outMesh);
    };
}
