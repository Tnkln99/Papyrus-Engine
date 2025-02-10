#include "Mrc/Exporters/AStaticModelExporter.h"

#include "Nmd/FileHelper.h"

#include <filesystem>
#include <fstream>

namespace Mrc
{
    void AStaticModelExporter::exportAsStaticModel(const AStaticModel &staticModel, const std::string &dirName, const std::string &fileName)
    {
        flatbuffers::FlatBufferBuilder builder;

        // Convert models to FlatBuffer format
        const auto flatModel = convertModel(builder, staticModel);

        // Finish building the FlatBuffer
        builder.Finish(flatModel);

        const std::string pathDir = Nmd::FileHelper::constructFilePathWithExtension(dirName, fileName, ".asm");

        // Write out FlatBuffer data to file
        std::ofstream outFile(pathDir, std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    }

    flatbuffers::Offset<Mrc::StaticModel> AStaticModelExporter::convertModel(flatbuffers::FlatBufferBuilder &builder,
        const AStaticModel &model)
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

    flatbuffers::Offset<Mrc::StaticMesh> AStaticModelExporter::convertMesh(flatbuffers::FlatBufferBuilder &builder,
        const AStaticMesh &mesh)
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
