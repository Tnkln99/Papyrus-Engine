#pragma once

#include "Arf/Data/StaticModel.h"

#include <string>

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Mrc
{
    class AssimpImporter final
    {
    public:
        void import(const std::string &filePath, Arf::StaticModel &outModel);

    private:
        void processScene(const aiScene* scene, Arf::StaticModel &outModel);

        aiNode *findFirstNodeWithMesh(aiNode *node);

        void processModel(const aiNode* node, const aiScene* scene, Arf::StaticModel& outModel);
        void processMesh(const aiMesh* mesh, Arf::StaticMesh& outMesh);
    };
}
