#pragma once

#include "Mrc/Data/AStaticModel.h"
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
        void getStaticModel(AStaticModel& outModel) const;
    private:
        std::unique_ptr<IImporter> m_importer;
        std::string m_filePath;
    };
}
