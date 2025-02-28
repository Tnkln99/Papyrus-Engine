#include "Mrc/Importer.h"

#include <filesystem>
#include <utility>

#include "Mrc/Importers/StaticModelImporter.h"
#include "Mrc/Importers/AssimpImporter.h"
#include "Nmd/FileHelper.h"


namespace Mrc
{
    Importer::Importer(std::string filePath) : m_filePath(std::move(filePath))
    {
        m_extension = Nmd::FileHelper::getFileExtension(m_filePath);
    }

    Importer::~Importer() = default;

    void Importer::getStaticModel(Arf::StaticModel& outModel) const
    {
        if (m_extension == "asm")
        {
            StaticModelImporter importer;
            importer.import(m_filePath, outModel);
        }
        else
        {
            AssimpImporter importer;
            importer.import(m_filePath, outModel);
        }
    }
}
