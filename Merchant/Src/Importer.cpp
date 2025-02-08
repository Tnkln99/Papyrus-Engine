#include "Mrc/Importer.h"

#include <filesystem>

#include "Mrc/Importers/AStaticModelImporter.h"
#include "Mrc/Importers/AssimpImporter.h"
#include "Nmd/FileHelper.h"


namespace Mrc
{
    Importer::Importer(const std::string &filPath)
    {
        const std::string extension = Nmd::FileHelper::getFileExtension(filPath);

        m_filePath = filPath;

        if (extension == "asm")
        {
            m_importer = std::make_unique<AStaticModelImporter>();
        }
        else
        {
            m_importer = std::make_unique<AssimpImporter>();
        }
    }

    Importer::~Importer() = default;

    void Importer::getStaticModel(AStaticModel& outModel) const
    {
        m_importer->import(m_filePath, outModel);
    }
}
