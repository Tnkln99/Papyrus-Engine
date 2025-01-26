#pragma once

#include <memory>
#include <string>

namespace Crv
{
    class PointCloudRenderer
    {
    public:
        PointCloudRenderer(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight);
        ~PointCloudRenderer();

        PointCloudRenderer(PointCloudRenderer&& other) = delete;
        PointCloudRenderer(const PointCloudRenderer& other) = delete;
        PointCloudRenderer& operator=(PointCloudRenderer&& other) = delete;
        PointCloudRenderer& operator=(const PointCloudRenderer& other) = delete;

        void init();
        void onResizeWindow(UINT width, UINT height) const;

        void updateCamera(const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projectionMatrix);
        void update(float dt);
        void render();

        uint32_t addPoint(const DirectX::XMFLOAT3 &position);
        void removePoint(uint32_t instanceId);

        void changePointLocation(uint32_t instanceId, const DirectX::XMFLOAT3 &position);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
