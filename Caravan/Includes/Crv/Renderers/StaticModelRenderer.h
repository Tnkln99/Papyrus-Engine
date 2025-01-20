#pragma once

#include <memory>
#include <string>

namespace Crv
{
    class StaticModelCreateInfo;

    class StaticModelRenderer
    {
    public:
        StaticModelRenderer(const HWND& hwnd, uint32_t displayWidth, uint32_t displayHeight);
        ~StaticModelRenderer();

        StaticModelRenderer(StaticModelRenderer&& other) = delete;
        StaticModelRenderer(const StaticModelRenderer& other) = delete;
        StaticModelRenderer& operator=(StaticModelRenderer&& other) = delete;
        StaticModelRenderer& operator=(const StaticModelRenderer& other) = delete;

        void init();
        void onResizeWindow(UINT width, UINT height) const;

        void updateCamera(const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projectionMatrix);
        void update(float dt);
        void render();

        // for now only one model it will change the model registered if used more then once
        void registerModel(const StaticModelCreateInfo &modelInfo);

        // returns instance id who has been created
        uint32_t addModelInstance(const DirectX::XMFLOAT4X4 &modelMatrix);
        void removeModelInstance(uint32_t instanceId);

        void setTransformStaticModelInstance(uint32_t instanceId, const DirectX::XMFLOAT4X4 &modelMatrix);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
