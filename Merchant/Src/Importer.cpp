#include "Mrc/Importer.h"

#include <filesystem>
#include <utility>

#include "Mrc/Importers/AStaticModelImporter.h"
#include "Mrc/Importers/AssimpImporter.h"
#include "Nmd/FileHelper.h"


namespace Mrc
{
    Importer::Importer(std::string filePath) : m_filePath(std::move(filePath))
    {
        const std::string extension = Nmd::FileHelper::getFileExtension(m_filePath);

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

    void Importer::getStaticModel(AStaticModel&outModel) const
    {
        m_importer->import(m_filePath, outModel);
    }
}
