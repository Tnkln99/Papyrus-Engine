#include "Mrc/Importers/AssimpImporter.h"

#include "Arf/Data/StaticModel.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Nmd/Logger.h"
#include "Nmd/Asserter.h"


namespace Mrc
{
    void AssimpImporter::import(const std::string &filePath, Arf::StaticModel &outModel)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate
                                                        | aiProcess_JoinIdenticalVertices
                                                        | aiProcess_OptimizeMeshes
                                                        | aiProcess_GenNormals
                                                        | aiProcess_CalcTangentSpace
                                                        | aiProcess_FlipUVs);

        if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, scene && scene->mRootNode && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE),
                         "Failed to load model: {}", importer.GetErrorString());
        }

        processScene(scene, outModel);
    }

    void AssimpImporter::processScene(const aiScene* scene, Arf::StaticModel& outModel)
    {
        // Safeguard checks
        if (!scene || !scene->mRootNode)
            return;

        // Find the first node in the scene hierarchy that contains a mesh
        if (const aiNode* firstMeshNode = findFirstNodeWithMesh(scene->mRootNode))
        {
            outModel.m_name = firstMeshNode->mName.data;
            // Process only that node (and any of its children if your processModel does recursion)
            processModel(firstMeshNode, scene, outModel);
        }
        else
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Failed to find mesh node");
        }
    }

    aiNode* AssimpImporter::findFirstNodeWithMesh(aiNode* node)
    {
        // If this node has at least one mesh, return it immediately
        if (node->mNumMeshes > 0)
        {
            return node;
        }

        // Otherwise, recurse into its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            if (aiNode* found = findFirstNodeWithMesh(node->mChildren[i]))
            {
                return found;
            }
        }

        // No mesh found in this branch
        return nullptr;
    }


    void AssimpImporter::processModel(const aiNode* node, const aiScene* scene, Arf::StaticModel& outModel)
    {
        // Process each mesh at this node
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Arf::StaticMesh staticMesh;
            staticMesh.m_name = mesh->mName.C_Str();
            processMesh(mesh, staticMesh);
            outModel.m_meshes.push_back(staticMesh);
        }
    }

    void AssimpImporter::processMesh(const aiMesh* mesh, Arf::StaticMesh& outMesh)
    {
        // Vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Arf::Vertex vertex{};

            vertex.m_position = DirectX::XMFLOAT3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);

            vertex.m_normal = DirectX::XMFLOAT3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z);

            if (mesh->mTextureCoords[0])
            {
                vertex.m_uv = DirectX::XMFLOAT2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.m_uv = DirectX::XMFLOAT2(0.0f, 0.0f);
            }

            outMesh.m_vertices.push_back(vertex);
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                outMesh.m_indices.push_back(face.mIndices[j]);
            }
        }
    }
} // namespace Mrc
