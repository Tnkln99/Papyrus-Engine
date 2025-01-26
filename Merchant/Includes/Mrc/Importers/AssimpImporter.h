#pragma once

#include "Mrc/Importers/IImporter.h"
#include "Mrc/Data/Assets.h"

#include <string>

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Mrc
{
    class AssimpImporter final : public IImporter
    {
    public:
        void import(const std::string &filePath, AScene &outScene) override;

    private:
        void processScene(const aiScene* scene, AScene &outScene);
        AStaticModel processModel(const aiNode* node, const aiScene* scene);
        AStaticMesh processMesh(const aiMesh* mesh, const aiScene* scene);

        void applySizeConstraint(AStaticModel &model);
    };
}
