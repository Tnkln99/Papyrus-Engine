#include "Os/WindowFactory.h"
#include "Crv/Renderers/HelloTriangleRenderer.h"
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
            Crv::GeometryBufferCreateInfo bufferCreateInfo;

            bufferCreateInfo.m_indices = mesh.m_indices;
            bufferCreateInfo.m_vertexBuffer = mesh.m_vertices.data();
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

    // ReSharper disable once CppDFALocalValueEscapesFunction
    return results;
}

void setCameraPosition(Crv::StaticModelRenderer& renderer)
{
    // Define the camera position and target
    const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(-5.0f, 0.0f, 0.0f, 1.0f); // Camera at (-5, 0, 0)
    const DirectX::XMVECTOR targetPosition = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f); // Looking forward along +z
    const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Up is along +y

    // Create the view matrix
    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, targetPosition, upDirection);

    // Define the projection parameters
    const float fovAngleY = DirectX::XMConvertToRadians(45.0f); // 45-degree vertical field of view
    const float aspectRatio = 16.0f / 9.0f; // Common widescreen aspect ratio
    const float nearZ = 0.1f; // Near clipping plane
    const float farZ = 1000.0f; // Far clipping plane

    // Create the projection matrix
    const DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

    // Output matrices (for demonstration or debugging)
    DirectX::XMFLOAT4X4 viewMatrixOutput{}, projectionMatrixOutput{};
    XMStoreFloat4x4(&viewMatrixOutput, viewMatrix);
    XMStoreFloat4x4(&projectionMatrixOutput, projectionMatrix);

    renderer.updateCamera(viewMatrixOutput, projectionMatrixOutput);
}

int main()
{
    const auto window = Os::WindowFactory::createWindow(Os::OsType::Windows);

    if (!window->init("Papyrus", 800, 600))
    {
        return 1;
    }

    const auto& staticModels = importFileAsAScene("stanford-bunny.fbx");

    auto windowHandler = static_cast<HWND>(window->getHandler());
    auto renderer = std::make_unique<Crv::StaticModelRenderer>(windowHandler, 800, 600);
    renderer->init();
    renderer->changeModel(staticModels[0]);
    setCameraPosition(*renderer);

    window->show();

    // Window callbacks
    window->setMessageCallback([&renderer](const Os::Message& message) {
        switch (message.m_type)
        {
            case Os::MessageType::Paint:
                renderer->update(1); // todo: calculate dt
                renderer->render();
                break;
            case Os::MessageType::Resize:
                renderer->onResizeWindow(message.m_resize.m_width, message.m_resize.m_height);
                break;
            case Os::MessageType::Close:
                NOMAD_LOG(Nmd::LogLevel::Info, "Closing...");
                break;
            default:
                break;
        }
    });

    while (!window->shouldClose())
    {
        window->processMessage();
    }

    return 0;
}
