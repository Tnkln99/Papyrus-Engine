#include "Mrc/Importers/StaticModelImporter.h"

#include "Nmd/Asserter.h"

#include <fstream>
#include <ios>


namespace Mrc
{
    void StaticModelImporter::import(const std::string &filePath, Arf::StaticModel& outModel)
    {
        return importStaticModel(filePath, outModel);
    }

    void StaticModelImporter::importStaticModel(const std::string &filePath, Arf::StaticModel& outModel)
    {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile)
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, false, "File not found");
        }

        const std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(inFile), {});
        const auto* flatModel = Arf::Fbs::GetStaticModel(buffer.data());

        return processModel(flatModel, outModel);
    }

    void StaticModelImporter::processModel(const Arf::Fbs::StaticModel* flatModel, Arf::StaticModel& outModel)
    {
        for (const auto* flatMesh : *flatModel->meshes())
        {
            Arf::StaticMesh mesh;
            mesh.m_name = flatMesh->name()->c_str();
            processMesh(flatMesh, mesh);
            outModel.m_meshes.push_back(mesh);
            outModel.m_name = flatModel->name()->c_str();
        }
    }

    void StaticModelImporter::processMesh(const Arf::Fbs::StaticMesh* flatMesh, Arf::StaticMesh& outMesh)
    {
        for (const auto* flatVertex : *flatMesh->vertices())
        {
            Arf::Vertex vertex{};
            vertex.m_position = DirectX::XMFLOAT3(flatVertex->position()->Get(0), flatVertex->position()->Get(1), flatVertex->position()->Get(2));
            vertex.m_normal = DirectX::XMFLOAT3(flatVertex->normal()->Get(0), flatVertex->normal()->Get(1), flatVertex->normal()->Get(2));
            vertex.m_uv = DirectX::XMFLOAT2(flatVertex->uv()->Get(0), flatVertex->uv()->Get(1));

            outMesh.m_vertices.push_back(vertex);
        }

        outMesh.m_indices.assign(flatMesh->indices()->begin(), flatMesh->indices()->end());
    }
}
