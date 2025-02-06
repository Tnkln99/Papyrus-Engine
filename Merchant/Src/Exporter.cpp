#include "Mrc/Exporter.h"

#include <filesystem>
#include <flatbuffers/flatbuffers.h>
#include <fstream>

namespace Mrc
{
    void Exporter::exportStaticModel(const AStaticModel& staticModel, const std::string& fileDirectory, const std::string &fileName)
    {
        flatbuffers::FlatBufferBuilder builder;

        // Convert models to FlatBuffer format
        const auto flatModel = convertModel(builder, staticModel);

        // Finish building the FlatBuffer
        builder.Finish(flatModel);

        // Construct file path
        std::filesystem::path pathDir(fileDirectory);
        pathDir /= fileName;
        pathDir.replace_extension(".asm"); // Adjust extension as needed

        // Write out FlatBuffer data to file
        std::ofstream outFile(pathDir, std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    }

    flatbuffers::Offset<Mrc::StaticModel> Exporter::convertModel(flatbuffers::FlatBufferBuilder& builder, const AStaticModel& model)
    {
        std::vector<flatbuffers::Offset<Mrc::StaticMesh>> flatMeshes;
        flatMeshes.reserve(model.m_meshes.size());

        // Convert each mesh
        for (const auto& mesh : model.m_meshes)
        {
            flatMeshes.push_back(convertMesh(builder, mesh));
        }

        // Create and return a StaticModel offset
        return Mrc::CreateStaticModel(builder, builder.CreateVector(flatMeshes));
    }

    flatbuffers::Offset<Mrc::StaticMesh> Exporter::convertMesh(flatbuffers::FlatBufferBuilder& builder, const AStaticMesh& mesh)
    {
        std::vector<flatbuffers::Offset<Mrc::Vertex>> flatVertices;
        flatVertices.reserve(mesh.m_vertices.size());

        // Convert each vertex
        for (const auto& vertex : mesh.m_vertices)
        {
            auto position = builder.CreateVector(
                std::vector<float>{ vertex.m_position.x, vertex.m_position.y, vertex.m_position.z }
            );
            auto normal = builder.CreateVector(
                std::vector<float>{ vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z }
            );
            auto uv = builder.CreateVector(
                std::vector<float>{ vertex.m_uv.x, vertex.m_uv.y }
            );

            flatVertices.push_back(Mrc::CreateVertex(builder, position, normal, uv));
        }

        // Create indices vector
        auto indices = builder.CreateVector(mesh.m_indices);

        // Create and return a StaticMesh offset
        return Mrc::CreateStaticMesh(builder, builder.CreateVector(flatVertices), indices);
    }
}
