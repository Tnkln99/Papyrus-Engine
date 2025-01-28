#include <chrono>

#include "Nmd/Asserter.h"
#include "Os/WindowFactory.h"
#include "Crv/Renderers/StaticModelRenderer.h"
#include "Crv/Data/StaticModelCreateInfo.h"
#include "Nmd/Logger.h"
#include "Mrc/Importer.h"

#include <windows.h>
#include <iostream>

#include "SimpleCamera.h"
#include "Mrc/Exporter.h"


std::vector<Crv::StaticModelCreateInfo> importFileAsAScene(const std::string &directoryName, const std::string &fileName)
{
    std::vector<Crv::StaticModelCreateInfo> results;

    const Mrc::Importer importer(directoryName,fileName);
    Mrc::AStaticModel model;
    importer.getStaticModel(model);

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

    NOMAD_ASSERT(Nmd::AssertType::Expect, !results.empty(), "Invalid import path!");

    return results;
}

void setUpScene(Crv::StaticModelRenderer& renderer)
{
    constexpr float scale = 1.0f;

    const XMMATRIX mat = XMMatrixTranslation(2, 0, 0);
    const auto matT = XMMatrixTranspose(mat);

    XMFLOAT4X4 modelMatrixOut{};
    XMStoreFloat4x4(&modelMatrixOut, matT);

    auto id = renderer.addModelInstance(modelMatrixOut);

    const XMMATRIX mat2 = XMMatrixScaling(scale, scale, scale) * XMMatrixTranslation(-2, 0, 0);
    const XMMATRIX matT2 = XMMatrixTranspose(mat2);

    XMFLOAT4X4 modelMatrixOut2{};
    XMStoreFloat4x4(&modelMatrixOut2, matT2);

    auto id2 = renderer.addModelInstance(modelMatrixOut2);
}

void setCameraPosition(Crv::StaticModelRenderer& renderer, SimpleCamera& camera, const int width, const int height)
{
    const auto proj = camera.getProjectionMatrix(45, width/height, 0.1f, 1000.0f);
    const auto transposeProjMat = XMMatrixTranspose(proj);

    const auto view = camera.getViewMatrix();
    const auto transposeViewMat = XMMatrixTranspose(view);


    XMFLOAT4X4 viewOut{}, projOut{};
    XMStoreFloat4x4(&viewOut, transposeViewMat);
    XMStoreFloat4x4(&projOut, transposeProjMat);

    renderer.updateCamera(viewOut, projOut);
}

int main()
{
    const auto window = Os::WindowFactory::createWindow(Os::OsType::Windows);

    int windowHeight = 600;
    int windowWidth = 800;
    if (!window->init("Papyrus", windowWidth, windowHeight))
    {
        return 1;
    }

    const auto& staticModels = importFileAsAScene("", "Suzanne.fbx");

    auto windowHandler = static_cast<HWND>(window->getHandler());
    auto renderer = std::make_unique<Crv::StaticModelRenderer>(windowHandler, 800, 600);
    renderer->registerModel(staticModels[0]);
    renderer->init();

    setUpScene(*renderer);

    SimpleCamera camera{};
    camera.init({0,0,-5});
    camera.setMoveSpeed(10.0f);

    // Window callbacks
    window->setMessageCallback([&renderer, &windowWidth, &windowHeight, &camera](const Os::Message& message) {
        switch (message.m_type)
        {
            case Os::MessageType::Resize:
                windowHeight = message.m_resize.m_height;
                windowWidth = message.m_resize.m_width;
                renderer->onResizeWindow(message.m_resize.m_width, message.m_resize.m_height);
                break;
            case Os::MessageType::Close:
                NOMAD_LOG(Nmd::LogLevel::Info, "Closing...");
                break;
            case Os::MessageType::KeyDown:
                camera.onKeyDown(message.m_keyDownId);
                break;
            case Os::MessageType::KeyUp:
                camera.onKeyUp(message.m_keyUpId);
                break;
            default:
                break;
        }
    });

    window->show();

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!window->shouldClose())
    {
        // Calculate elapsed time (dt)
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        float dt = elapsed.count(); // dt in seconds
        lastTime = currentTime;     // Update lastTime for the next frame

        window->processMessage();

        camera.update(dt);
        renderer->update(dt); // todo: calculate dt
        setCameraPosition(*renderer, camera, windowWidth, windowHeight);
        renderer->render();
    }

    return 0;
}
