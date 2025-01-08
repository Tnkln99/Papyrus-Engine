#include "Mrc/Importer.h"

#include "Mrc/Data/Assets.h"

#include "Nmd/Asserter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Mrc
{
    Importer::Importer(std::string filePath) : m_filePath(std::move(filePath))
    {
        import();
    }

    const AScene& Importer::getScene() const
    {
        return m_scene;
    }

    void Importer::import()
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(m_filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

        if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        {
            NOMAD_ASSERT(Nmd::AssertType::Assert, scene && scene->mRootNode && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE), "Failed to load model: {}", importer.GetErrorString());
        }

        processScene(scene);
    }

    void Importer::processScene(const aiScene* scene)
    {
        if (scene->mRootNode)
        {
            m_scene.m_models.push_back(processModel(scene->mRootNode, scene));
        }
    }

    AStaticModel Importer::processModel(const aiNode* node, const aiScene* scene)
    {
        AStaticModel model;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.m_meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            AStaticModel childModel = processModel(node->mChildren[i], scene);
            model.m_meshes.insert(model.m_meshes.end(), childModel.m_meshes.begin(), childModel.m_meshes.end());
        }

        return model;
    }

    AStaticMesh Importer::processMesh(const aiMesh* mesh, const aiScene* scene)
    {
        AStaticMesh staticMesh;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            AVertex vertex{};

            vertex.m_position = DirectX::XMFLOAT4(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                1.0f);

            vertex.m_normal = DirectX::XMFLOAT4(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z,
                0.0f);

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
}
