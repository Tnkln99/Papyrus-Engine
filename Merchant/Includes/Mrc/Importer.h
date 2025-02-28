#pragma once

#include "Arf/Data/StaticModel.h"
#include <string>
#include <memory>

namespace Mrc
{
    class IImporter;

    class Importer
    {
    public:
        explicit Importer(std::string filePath);
        ~Importer();

        // import process happens here
        void getStaticModel(Arf::StaticModel& outModel) const;
    private:
        std::string m_filePath;
        std::string m_extension;
    };
}
