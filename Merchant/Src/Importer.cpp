#include "Mrc/Importer.h"

#include <filesystem>

#include "Mrc/Importers/AStaticModelImporter.h"
#include "Mrc/Importers/AssimpImporter.h"


namespace Mrc
{
    std::string getFileExtension(const std::string& fileName)
    {
        // Check if a dot was found and it's not the first character
        if (const size_t dotPosition = fileName.find_last_of('.'); dotPosition != std::string::npos && dotPosition != 0)
        {
            return fileName.substr(dotPosition + 1); // Extract and return the extension
        }

        // If no dot is found or it's at the first position, return an empty string
        return "";
    }

    Importer::Importer(const std::string &fileDirectory, const std::string& fileName)
    {
        const std::string extension = getFileExtension(fileName);

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
