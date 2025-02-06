#include "Mrc/Importer.h"

#include <filesystem>

#include "Mrc/Importers/AStaticModelImporter.h"
#include "Mrc/Importers/AssimpImporter.h"
#include "Nmd/FileHelper.h"


namespace Mrc
{
    Importer::Importer(const std::string &fileDirectory, const std::string& fileName)
    {
        const std::string extension = Nmd::FileHelper::getFileExtension(fileName);

        std::filesystem::path pathDir(fileDirectory);
        pathDir /= fileName;

        m_filePath = pathDir.string();

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
