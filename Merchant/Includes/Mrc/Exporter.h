#pragma once

#undef max // Undefine Windows' max macro
#include "Mrc/Generated/AScene_generated.h"
#include "Mrc/Data/Assets.h"
#include <string>

namespace Mrc
{
    class Exporter
    {
    public:
        static void exportScene(const AScene& scene, const std::string& fileDirectory, std::string fileName);

    private:
        static flatbuffers::Offset<Mrc::StaticModel> convertModel(flatbuffers::FlatBufferBuilder& builder, const AStaticModel& model);
        static flatbuffers::Offset<Mrc::StaticMesh> convertMesh(flatbuffers::FlatBufferBuilder& builder, const AStaticMesh& mesh);
    };
}
