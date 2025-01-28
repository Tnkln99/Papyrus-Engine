#pragma once

#include "Mrc/Importers/IImporter.h"
#include "Mrc/Data/AStaticModel.h"

#include <string>

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Mrc
{
    class AssimpImporter final : public IImporter
    {
    public:
        void import(const std::string &filePath, AStaticModel &outModel) override;

    private:
        void processScene(const aiScene* scene, AStaticModel &outModel);

        aiNode *findFirstNodeWithMesh(aiNode *node);

        void processModel(const aiNode* node, const aiScene* scene, AStaticModel& outModel);
        void processMesh(const aiMesh* mesh, AStaticMesh& outMesh);
    };
}
