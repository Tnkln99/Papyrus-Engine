#include "Mrc/Importers/AStaticModelImporter.h"

#include "Nmd/Asserter.h"

#include <fstream>
#include <ios>


namespace Mrc
{
    void AStaticModelImporter::import(const std::string &filePath, AStaticModel& outScene)
    {
        return importStaticModel(filePath, outScene);
    }

    void AStaticModelImporter::importStaticModel(const std::string &filePath, AStaticModel& outScene)
    {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile)
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, false, "File not found");
        }

        const std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(inFile), {});
        const auto* flatModel = Mrc::GetStaticModel(buffer.data());

        return processModel(flatModel, outScene);
    }

    void AStaticModelImporter::processModel(const Mrc::StaticModel* flatModel, AStaticModel& outModel)
    {
        for (const auto* flatMesh : *flatModel->meshes())
        {
            AStaticMesh mesh;
            processMesh(flatMesh, mesh);
            outModel.m_meshes.push_back(mesh);
        }
    }

    void AStaticModelImporter::processMesh(const Mrc::StaticMesh* flatMesh, AStaticMesh& outMesh)
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
