#include "Mrc/Exporter.h"

#include <filesystem>
#include <flatbuffers/flatbuffers.h>
#include <fstream>

namespace Mrc
{
    void Exporter::exportScene(const AScene& scene, const std::string& fileDirectory, std::string fileName)
    {
        flatbuffers::FlatBufferBuilder builder;

        // Convert models to FlatBuffer format
        std::vector<flatbuffers::Offset<Mrc::StaticModel>> flatModels;
        flatModels.reserve(scene.m_models.size());
        for (const auto& model : scene.m_models)
        {
            flatModels.push_back(convertModel(builder, model));
        }

        const auto flatScene = Mrc::CreateScene(builder, builder.CreateVector(flatModels));
        builder.Finish(flatScene);

        std::filesystem::path pathDir(fileDirectory);
        pathDir /= fileName;
        pathDir.replace_extension(".ascene");

        std::ofstream outFile(pathDir, std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    }

    flatbuffers::Offset<Mrc::StaticModel> Exporter::convertModel(flatbuffers::FlatBufferBuilder& builder, const AStaticModel& model)
    {
        std::vector<flatbuffers::Offset<Mrc::StaticMesh>> flatMeshes;
        flatMeshes.reserve(model.m_meshes.size());
        for (const auto& mesh : model.m_meshes)
        {
            flatMeshes.push_back(convertMesh(builder, mesh));
        }

        return Mrc::CreateStaticModel(builder, builder.CreateVector(flatMeshes));
    }

    flatbuffers::Offset<Mrc::StaticMesh> Exporter::convertMesh(flatbuffers::FlatBufferBuilder& builder, const AStaticMesh& mesh)
    {
        std::vector<flatbuffers::Offset<Mrc::Vertex>> flatVertices;
        for (const auto& vertex : mesh.m_vertices)
        {
            const auto position = builder.CreateVector(std::vector<float>{vertex.m_position.x, vertex.m_position.y, vertex.m_position.z});
            const auto normal = builder.CreateVector(std::vector<float>{vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z});
            const auto uv = builder.CreateVector(std::vector<float>{vertex.m_uv.x, vertex.m_uv.y});

            flatVertices.push_back(Mrc::CreateVertex(builder, position, normal, uv));
        }

        const auto indices = builder.CreateVector(mesh.m_indices);

        return Mrc::CreateStaticMesh(builder, builder.CreateVector(flatVertices), indices);
    }
}
