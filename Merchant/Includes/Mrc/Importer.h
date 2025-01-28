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
        explicit Importer(const std::string &fileDirectory, const std::string& fileName);
        ~Importer();

        // import process happens here
        void getStaticModel(AStaticModel& outModel) const;
    private:
        std::unique_ptr<IImporter> m_importer;
        std::string m_filePath;
    };
}
