#pragma once

#include <string>

#include "Data/Assets.h"

struct aiMesh;
struct aiNode;
struct aiScene;

namespace Mrc
{
    class Importer
    {
    public:
        explicit Importer(std::string filePath);
        [[nodiscard]] const AScene& getScene() const;

    private:
        void import();
        void processScene(const aiScene* scene);
        AStaticModel processModel(const aiNode* node, const aiScene* scene);
        AStaticMesh processMesh(const aiMesh* mesh, const aiScene* scene);

        std::string m_filePath;
        AScene m_scene;
    };
}
