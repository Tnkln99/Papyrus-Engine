#include "Mrc/Exporters/AStaticModelExporter.h"

#include "Nmd/FileHelper.h"

#include <filesystem>
#include <fstream>

namespace Mrc
{
    void AStaticModelExporter::exportAsStaticModel(const Arf::StaticModel &staticModel, const std::string &dirName, const std::string &fileName)
    {
        flatbuffers::FlatBufferBuilder builder;

        // Convert models to FlatBuffer format
        const auto flatModel = convertModel(builder, staticModel);

        // Finish building the FlatBuffer
        builder.Finish(flatModel);

        const std::string pathDir = Nmd::FileHelper::constructFilePathWithExtension(dirName, fileName, ".sm");

        // Write out FlatBuffer data to file
        std::ofstream outFile(pathDir, std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    }

    flatbuffers::Offset<Arf::Fbs::StaticModel> AStaticModelExporter::convertModel(flatbuffers::FlatBufferBuilder &builder,
        const Arf::StaticModel &model)
    {
        std::vector<flatbuffers::Offset<Arf::Fbs::StaticMesh>> flatMeshes;
        flatMeshes.reserve(model.m_meshes.size());

        // Convert each mesh
        for (const auto& mesh : model.m_meshes)
        {
            flatMeshes.push_back(convertMesh(builder, mesh));
        }
        const auto modelName = builder.CreateString(model.m_name);
        // Create and return a StaticModel offset
        return Arf::Fbs::CreateStaticModel(builder, modelName, builder.CreateVector(flatMeshes));
    }

    flatbuffers::Offset<Arf::Fbs::StaticMesh> AStaticModelExporter::convertMesh(flatbuffers::FlatBufferBuilder &builder,
        const Arf::StaticMesh &mesh)
    {
        std::vector<flatbuffers::Offset<Arf::Fbs::Vertex>> flatVertices;
        flatVertices.reserve(mesh.m_vertices.size());

        // Convert each vertex
        for (const auto& vertex : mesh.m_vertices)
        {
            const auto position = builder.CreateVector(
                std::vector<float>{ vertex.m_position.x, vertex.m_position.y, vertex.m_position.z }
            );
            const auto normal = builder.CreateVector(
                std::vector<float>{ vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z }
            );
            const auto uv = builder.CreateVector(
                std::vector<float>{ vertex.m_uv.x, vertex.m_uv.y }
            );

            flatVertices.push_back(Arf::Fbs::CreateVertex(builder, position, normal, uv));
        }

        // Create indices vector
        const auto indices = builder.CreateVector(mesh.m_indices);
        const auto meshName = builder.CreateString(mesh.m_name);

        // Create and return a StaticMesh offset
        return Arf::Fbs::CreateStaticMesh(builder, meshName, builder.CreateVector(flatVertices), indices);
    }
}
