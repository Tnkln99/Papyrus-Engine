#include "Mrc/Importers/ASceneImporter.h"

#include <fstream>
#include <ios>

#include "Nmd/Asserter.h"

namespace Mrc
{
    void ASceneImporter::import(const std::string &filePath, AScene& outScene)
    {
        return importScene(filePath, outScene);
    }

    void ASceneImporter::importScene(const std::string &filePath, AScene& outScene)
    {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile)
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, false, "File not found");
        }

        const std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(inFile), {});
        const auto* flatScene = Mrc::GetScene(buffer.data());

        return processScene(flatScene, outScene);
    }

    void ASceneImporter::processScene(const Mrc::Scene *flatScene, AScene& outScene)
    {
        for (const auto* flatModel : *flatScene->models())
        {
            AStaticModel model;
            processModel(flatModel, model);
            outScene.m_models.push_back(model);
        }
    }

    void ASceneImporter::processModel(const Mrc::StaticModel* flatModel, AStaticModel& outModel)
    {
        for (const auto* flatMesh : *flatModel->meshes())
        {
            AStaticMesh mesh;
            processMesh(flatMesh, mesh);
            outModel.m_meshes.push_back(mesh);
        }
    }

    void ASceneImporter::processMesh(const Mrc::StaticMesh* flatMesh, AStaticMesh& outMesh)
    {
        for (const auto* flatVertex : *flatMesh->vertices())
        {
            AVertex vertex{};
            vertex.m_position = DirectX::XMFLOAT3(flatVertex->position()->Get(0), flatVertex->position()->Get(1), flatVertex->position()->Get(2));
            vertex.m_normal = DirectX::XMFLOAT3(flatVertex->normal()->Get(0), flatVertex->normal()->Get(1), flatVertex->normal()->Get(2));
            vertex.m_uv = DirectX::XMFLOAT2(flatVertex->uv()->Get(0), flatVertex->uv()->Get(1));

            outMesh.m_vertices.push_back(vertex);
        }

        outMesh.m_indices.assign(flatMesh->indices()->begin(), flatMesh->indices()->end());
    }
}
