#include "Mrc/Importers/AssimpImporter.h"

#include "Mrc/Data/Assets.h"
#include "Nmd/Asserter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm> // for std::min, std::max
#include <float.h>   // for FLT_MAX, -FLT_MAX

namespace Mrc
{
    void AssimpImporter::import(const std::string &filePath, AScene &outScene)
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

        processScene(scene, outScene);
    }

    void AssimpImporter::processScene(const aiScene* scene, AScene &outScene)
    {
        if (scene->mRootNode)
        {
            // Process the root node (and all of its children)
            AStaticModel rootModel = processModel(scene->mRootNode, scene);

            // Here you have the complete model of the scene; push it into outScene
            outScene.m_models.push_back(rootModel);
        }
    }

    AStaticModel AssimpImporter::processModel(const aiNode* node, const aiScene* scene)
    {
        AStaticModel model;

        // Process each mesh at this node
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.m_meshes.push_back(processMesh(mesh, scene));
        }

        // Recursively process children
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            AStaticModel childModel = processModel(node->mChildren[i], scene);

            // Merge child meshes into the current model
            model.m_meshes.insert(model.m_meshes.end(),
                                  childModel.m_meshes.begin(),
                                  childModel.m_meshes.end());
        }

        // After collecting all meshes (self + children), ensure bounding box constraints
        applySizeConstraint(model);

        return model;
    }

    AStaticMesh AssimpImporter::processMesh(const aiMesh* mesh, const aiScene* /*scene*/)
    {
        AStaticMesh staticMesh;

        // Vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            AVertex vertex{};

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

            staticMesh.m_vertices.push_back(vertex);
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                staticMesh.m_indices.push_back(face.mIndices[j]);
            }
        }

        return staticMesh;
    }

    void AssimpImporter::applySizeConstraint(AStaticModel& model)
    {
        // Compute bounding box
        DirectX::XMFLOAT3 minBounds( FLT_MAX,  FLT_MAX,  FLT_MAX);
        DirectX::XMFLOAT3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        bool hasVertices = false;

        // Gather min/max over all meshes
        for (auto& mesh : model.m_meshes)
        {
            for (auto& vertex : mesh.m_vertices)
            {
                hasVertices = true;
                minBounds.x = std::min(minBounds.x, vertex.m_position.x);
                minBounds.y = std::min(minBounds.y, vertex.m_position.y);
                minBounds.z = std::min(minBounds.z, vertex.m_position.z);

                maxBounds.x = std::max(maxBounds.x, vertex.m_position.x);
                maxBounds.y = std::max(maxBounds.y, vertex.m_position.y);
                maxBounds.z = std::max(maxBounds.z, vertex.m_position.z);
            }
        }

        // Only proceed if we actually have vertices
        if (!hasVertices)
            return;

        // Calculate the size of the bounding box
        float sizeX = maxBounds.x - minBounds.x;
        float sizeY = maxBounds.y - minBounds.y;
        float sizeZ = maxBounds.z - minBounds.z;

        // Find the largest dimension
        float maxDimension = std::max(sizeX, std::max(sizeY, sizeZ));

        // Define your minimum and maximum bounding box sizes
        const float minAllowedSize = 1.0f;  // Example
        const float maxAllowedSize = 10.0f; // Example

        // Determine if scaling is needed
        float scaleFactor = 1.0f;
        if (maxDimension < minAllowedSize)
        {
            scaleFactor = minAllowedSize / maxDimension;
        }
        else if (maxDimension > maxAllowedSize)
        {
            scaleFactor = maxAllowedSize / maxDimension;
        }

        // Uniform scale all vertices if scaleFactor != 1.0f
        if (fabs(scaleFactor - 1.0f) > 0.0001f)
        {
            for (auto& mesh : model.m_meshes)
            {
                for (auto& vertex : mesh.m_vertices)
                {
                    // Scale position around the origin (0,0,0)
                    vertex.m_position.x *= scaleFactor;
                    vertex.m_position.y *= scaleFactor;
                    vertex.m_position.z *= scaleFactor;
                }
            }
        }
    }

} // namespace Mrc
