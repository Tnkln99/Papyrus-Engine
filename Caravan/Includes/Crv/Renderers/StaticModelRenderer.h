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
        void render() const;

        void registerModel(const StaticModelCreateInfo &modelInfo);
        void unregisterModel(const std::string &name);

        // returns instance id who has been created
        uint32_t addModelInstance(const std::string &m_modelName, DirectX::XMFLOAT4X4 modelMatrix);
        void removeModelInstance(const std::string &m_modelName, uint32_t instanceId);

        void setTransformStaticModelInstance(const std::string& m_modelName, uint32_t instanceId, const DirectX::XMFLOAT4X4 &modelMatrix);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
