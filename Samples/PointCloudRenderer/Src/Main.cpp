#include "SimpleCamera.h"

#include "Nmd/Logger.h"

#include "Os/WindowFactory.h"
#include "Crv/Renderers/PointCloudRenderer.h"

#include "Mrc/Importer.h"
#include "Mrc/Data/AStaticModel.h"
#include "Mrc/Exporter.h"


#include <windows.h>
#include <iostream>
#include <chrono>


void importFileAsPointCloud(Crv::PointCloudRenderer& renderer, const std::string &filaPath)
{
    const Mrc::Importer modelImporter(filaPath);
    Mrc::AStaticModel model;
    modelImporter.getStaticModel(model);

    Mrc::Exporter::exportStaticModel(model, "", "Maymun");

    const Mrc::Importer asmImporter("Maymun.asm");
    Mrc::AStaticModel model2;
    asmImporter.getStaticModel(model2);

    for (const Mrc::AStaticMesh & mesh : model2.m_meshes)
    {
        std::vector<uint8_t> vertexBuffer;
        vertexBuffer.resize(mesh.m_vertices.size() * sizeof(Mrc::AVertex));
        std::memcpy(vertexBuffer.data(), mesh.m_vertices.data(), vertexBuffer.size());

        for (const auto& vertex : mesh.m_vertices)
        {
            const DirectX::XMFLOAT3 pos = vertex.m_position;
            renderer.addPoint(XMFLOAT3(pos.x, pos.y, pos.z));
        }
    }
}

void createSpherePointCloud(Crv::PointCloudRenderer& renderer)
{
    constexpr int verticalDivisions = 1000; // Number of vertical slices (latitude)
    // ReSharper disable once CppTooWideScope
    constexpr int horizontalDivisions = 1000;

    for (int i = 0; i <= verticalDivisions; ++i)
    {
        // Latitude angle (phi) from -PI/2 to PI/2
        const float phi = XM_PI * (static_cast<float>(i) / verticalDivisions - 0.5f);

        for (int j = 0; j < horizontalDivisions; ++j)
        {
            constexpr float radius = 10.0f;
            // Longitude angle (theta) from 0 to 2PI
            const float theta = XM_2PI * static_cast<float>(j) / horizontalDivisions;

            // Convert spherical coordinates to Cartesian coordinates
            const float x = radius * cos(phi) * cos(theta);
            const float y = radius * cos(phi) * sin(theta);
            const float z = radius * sin(phi);

            // Add the point to the renderer
            renderer.addPoint(XMFLOAT3(x, y, z));
        }
    }
}

void setCameraPosition(Crv::PointCloudRenderer& renderer, SimpleCamera& camera, const int width, const int height)
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


    auto windowHandler = static_cast<HWND>(window->getHandler());
    auto renderer = std::make_unique<Crv::PointCloudRenderer>(windowHandler, 800, 600);
    renderer->init();

    importFileAsPointCloud(*renderer, "Suzanne.fbx");

    SimpleCamera camera{};
    camera.init({0,0,-10});
    camera.setMoveSpeed(5.0f);

    // Window callbacks
    window->setMessageCallback([&renderer, &windowWidth, &windowHeight, &camera](const Os::Message& message) {
        switch (message.m_type)
        {
            case Os::MessageType::Paint:
                break;
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
        const float dt = elapsed.count(); // dt in seconds
        lastTime = currentTime;     // Update lastTime for the next frame

        window->processMessage();

        camera.update(dt);
        renderer->update(dt); // todo: calculate dt
        setCameraPosition(*renderer, camera, windowWidth, windowHeight);
        renderer->render();
    }

    return 0;
}
