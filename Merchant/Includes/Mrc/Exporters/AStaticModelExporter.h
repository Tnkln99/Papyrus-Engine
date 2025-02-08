#pragma once
#include <Mrc/Generated/AStaticModel_generated.h>

#include "flatbuffers/buffer.h"
#include "Mrc/Data/AStaticModel.h"

namespace Mrc
{
    class AStaticModelExporter
    {
    public:
        void exportStaticModel(const AStaticModel& staticModel, const std::string &dirName, const std::string &fileName);
    private:
        flatbuffers::Offset<Mrc::StaticModel> convertModel(flatbuffers::FlatBufferBuilder& builder, const AStaticModel& model);
        flatbuffers::Offset<Mrc::StaticMesh> convertMesh(flatbuffers::FlatBufferBuilder& builder, const AStaticMesh& mesh);
    };
}
