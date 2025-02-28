#pragma once

#include "Arf/Data/StaticModel.h"

#include <Arf/Generated/StaticModel_generated.h>
#include <flatbuffers/buffer.h>

namespace Mrc
{
    class AStaticModelExporter
    {
    public:
        void exportAsStaticModel(const Arf::StaticModel& staticModel, const std::string &dirName, const std::string &fileName);
    private:
        flatbuffers::Offset<Arf::Fbs::StaticModel> convertModel(flatbuffers::FlatBufferBuilder& builder, const Arf::StaticModel& model);
        flatbuffers::Offset<Arf::Fbs::StaticMesh> convertMesh(flatbuffers::FlatBufferBuilder& builder, const Arf::StaticMesh& mesh);
    };
}
