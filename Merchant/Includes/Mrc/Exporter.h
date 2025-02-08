#pragma once

#undef max // Undefine Windows' max macro
#include "Mrc/Generated/AStaticModel_generated.h"
#include "Mrc/Data/AStaticModel.h"
#include <string>

namespace Mrc
{
    class Exporter
    {
    public:
        Exporter() = delete;
        static void exportStaticModel(const AStaticModel& staticModel, const std::string& exportDirectory, const std::string& name);

    private:
        static flatbuffers::Offset<Mrc::StaticModel> convertModel(flatbuffers::FlatBufferBuilder& builder, const AStaticModel& model);
        static flatbuffers::Offset<Mrc::StaticMesh> convertMesh(flatbuffers::FlatBufferBuilder& builder, const AStaticMesh& mesh);
    };
}
