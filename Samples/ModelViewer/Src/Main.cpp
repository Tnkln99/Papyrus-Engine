#include "Os/WindowFactory.h"
#include "Crv/Renderers/StaticModelRenderer.h"
#include "Crv/Data/StaticModelCreateInfo.h"
#include "Nmd/Logger.h"
#include "Mrc/Importer.h"

#include <windows.h>
#include <iostream>

#include "Nmd/Asserter.h"


std::vector<Crv::StaticModelCreateInfo> importFileAsAScene(const std::string &filePath)
{
    std::vector<Crv::StaticModelCreateInfo> results;

    const Mrc::Importer importer(filePath);

    for (const Mrc::AScene& scene = importer.getScene(); const Mrc::AStaticModel& model : scene.m_models)
    {
        Crv::StaticModelCreateInfo modelCreateInfo;
        modelCreateInfo.m_name = model.m_name;
        for (const Mrc::AStaticMesh & mesh : model.m_meshes)
        {
            std::vector<uint8_t> vertexBuffer;
            vertexBuffer.resize(mesh.m_vertices.size() * sizeof(Mrc::AVertex));
            std::memcpy(vertexBuffer.data(), mesh.m_vertices.data(), vertexBuffer.size());

            Crv::GeometryBufferCreateInfo bufferCreateInfo;

            bufferCreateInfo.m_indices = mesh.m_indices;
            bufferCreateInfo.m_vertexBuffer = vertexBuffer;
            bufferCreateInfo.m_vertexCount = mesh.m_vertices.size();
            bufferCreateInfo.m_vertexSize = sizeof(Mrc::AVertex);

            Crv::StaticMeshCreateInfo meshCreateInfo;

            meshCreateInfo.m_name = mesh.m_name;
            meshCreateInfo.m_verticesInfo = bufferCreateInfo;

            modelCreateInfo.m_meshes.push_back(meshCreateInfo);
        }
        results.push_back(modelCreateInfo);
    }

    NOMAD_ASSERT(Nmd::AssertType::Expect, !results.empty(), "Invalid import path!");

    return results;
}

void setCameraPosition(Crv::StaticModelRenderer& renderer, const int width, const int height)
{
    // Camera at (0, 0, –10), looking toward (0, 0, 0) in LH coords
    const DirectX::XMVECTOR eyePosition    = DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
    const DirectX::XMVECTOR targetPosition = DirectX::XMVectorSet(0.0f, 0.0f,  0.0f, 1.0f);
    const DirectX::XMVECTOR upDirection    = DirectX::XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f);

    const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, targetPosition, upDirection);

    auto transposeViewMat = DirectX::XMMatrixTranspose(viewMatrix);

    // 45° vertical FOV, 16:9, near=0.1f, far=1000.f
    constexpr float fovAngleY = DirectX::XMConvertToRadians(45.0f);
    const float aspect    = width / height;
    constexpr float nearZ     = 0.1f;
    constexpr float farZ      = 1000.0f;

    const DirectX::XMMATRIX projectionMatrix =
        DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspect, nearZ, farZ);

    auto transposeProjMat = DirectX::XMMatrixTranspose(projectionMatrix);

    // Store these into your constant buffer (row‐major in typical D3D style)
    DirectX::XMFLOAT4X4 viewOut, projOut;
    XMStoreFloat4x4(&viewOut, transposeViewMat);
    XMStoreFloat4x4(&projOut, transposeProjMat);

    // Now upload modelOut, viewOut, projOut to your CB
    renderer.updateCamera(viewOut, projOut);
}

int main()
{
    const auto window = Os::WindowFactory::createWindow(Os::OsType::Windows);

    int height = 600;
    int width = 800;
    if (!window->init("Papyrus", width, height))
    {
        return 1;
    }

    const auto& staticModels = importFileAsAScene("Cube.fbx");

    auto windowHandler = static_cast<HWND>(window->getHandler());
    auto renderer = std::make_unique<Crv::StaticModelRenderer>(windowHandler, 800, 600);
    renderer->registerModel(staticModels[0]);
    renderer->init();

    constexpr float scale = 1.0f;

    const DirectX::XMMATRIX mat = DirectX::XMMatrixTranslation(2, 0, 0);
    const auto matT = XMMatrixTranspose(mat);

    DirectX::XMFLOAT4X4 modelMatrixOut;
    XMStoreFloat4x4(&modelMatrixOut, matT);

    auto id = renderer->addModelInstance(modelMatrixOut);

    const DirectX::XMMATRIX mat2 = DirectX::XMMatrixScaling(scale, scale, scale) * DirectX::XMMatrixTranslation(-2, 0, 0);
    const DirectX::XMMATRIX matT2 = XMMatrixTranspose(mat2);

    DirectX::XMFLOAT4X4 modelMatrixOut2;
    XMStoreFloat4x4(&modelMatrixOut2, matT2);

    auto id2 = renderer->addModelInstance(modelMatrixOut2);

    window->show();

    // Window callbacks
    window->setMessageCallback([&renderer, &width, &height](const Os::Message& message) {
        switch (message.m_type)
        {
            case Os::MessageType::Paint:
                break;
            case Os::MessageType::Resize:
                height = message.m_resize.m_height;
                width = message.m_resize.m_width;
                renderer->onResizeWindow(message.m_resize.m_width, message.m_resize.m_height);
                break;
            case Os::MessageType::Close:
                NOMAD_LOG(Nmd::LogLevel::Info, "Closing...");
                break;
            case Os::MessageType::KeyDown:
                NOMAD_LOG(Nmd::LogLevel::Info, "{}", message.m_keyId);
                break;
            default:
                break;
        }
    });

    while (!window->shouldClose())
    {
        window->processMessage();

        renderer->update(1); // todo: calculate dt
        setCameraPosition(*renderer, width, height);
        renderer->render();
    }

    return 0;
}
