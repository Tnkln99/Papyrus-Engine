#pragma once

namespace Arf
{
    struct StaticMesh;
    struct VirtualStaticMesh;
}

namespace Atm
{
    class VsmMeshProcessor
    {
    public:
        void processMesh(const Arf::StaticMesh& mesh, const Arf::VirtualStaticMesh& outVirtualStaticMesh);

    private:
        void process();
    };
}
