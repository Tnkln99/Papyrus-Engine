#include "Nmd/FileHelper.h"
#include "Nmd/Logger.h"


namespace Nmd
{
    std::vector<std::string> FileHelper::listFilesWithExtension(const std::string& directory, const std::string& extension)
    {
        std::vector<std::string> files;
        try
        {
            NOMAD_LOG(Nmd::LogLevel::Info, "Listing files in directory: {}", directory);
            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file() && entry.path().extension() == extension)
                {
                    files.push_back(entry.path().string());
                }
            }
            NOMAD_LOG(Nmd::LogLevel::Message, "Found {} files with extension '{}'", files.size(), extension);
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error listing files: {}", e.what());
        }
        return files;
    }

    std::vector<std::string> FileHelper::listFilesRecursiveWithExtension(const std::string& directory, const std::string& extension)
    {
        std::vector<std::string> files;
        try {
            NOMAD_LOG(Nmd::LogLevel::Info, "Recursively listing files in directory: {}", directory);
            for (const auto& entry : fs::recursive_directory_iterator(directory))
            {
                if (entry.is_regular_file() && entry.path().extension() == extension)
                {
                    files.push_back(entry.path().string());
                }
            }
            NOMAD_LOG(Nmd::LogLevel::Message, "Found {} files with extension '{}' recursively", files.size(), extension);
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error listing files recursively: {}", e.what());
        }
        return files;
    }

    std::vector<std::string> FileHelper::listFiles(const std::string &directory)
    {
        std::vector<std::string> files;
        try
        {
            NOMAD_LOG(Nmd::LogLevel::Info, "Listing files in directory: {}", directory);
            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file())
                {
                    files.push_back(entry.path().string());
                }
            }
            NOMAD_LOG(Nmd::LogLevel::Message, "Found {} on folder {}", files.size(), directory);
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error listing files: {}", e.what());
        }
        return files;
    }

    bool FileHelper::fileExists(const std::string& filePath)
    {
        const bool exists = fs::exists(filePath);
        NOMAD_LOG(Nmd::LogLevel::Info, "Checking if file exists '{}': {}", filePath, exists ? "Yes" : "No");
        return exists;
    }

    bool FileHelper::deleteFile(const std::string& filePath)
    {
        try
        {
            if (fs::remove(filePath))
            {
                NOMAD_LOG(Nmd::LogLevel::Message, "Deleted file: {}", filePath);
                return true;
            }
            NOMAD_LOG(Nmd::LogLevel::Warning, "File not found for deletion: {}", filePath);
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error deleting file '{}': {}", filePath, e.what());
        }
        return false;
    }

    uintmax_t FileHelper::getFileSize(const std::string& filePath)
    {
        try
        {
            if (fs::exists(filePath) && fs::is_regular_file(filePath))
            {
                uintmax_t size = fs::file_size(filePath);
                NOMAD_LOG(Nmd::LogLevel::Message, "File size of '{}': {} bytes", filePath, size);
                return size;
            }
            NOMAD_LOG(Nmd::LogLevel::Warning, "File does not exist or is not a regular file: {}", filePath);
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error getting file size '{}': {}", filePath, e.what());
        }
        return 0;
    }

    bool FileHelper::createDirectory(const std::string& dirPath)
    {
        try
        {
            if (fs::create_directories(dirPath))
            {
                NOMAD_LOG(Nmd::LogLevel::Message, "Directory created: {}", dirPath);
                return true;
            }
            NOMAD_LOG(Nmd::LogLevel::Warning, "Directory already exists or could not be created: {}", dirPath);
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error creating directory '{}': {}", dirPath, e.what());
        }
        return false;
    }

    std::string FileHelper::getFileExtension(const std::string &fileName)
    {
        // Check if a dot was found and it's not the first character
        if (const size_t dotPosition = fileName.find_last_of('.'); dotPosition != std::string::npos && dotPosition != 0)
        {
            return fileName.substr(dotPosition + 1); // Extract and return the extension
        }

        // If no dot is found or it's at the first position, return an empty string
        return "";
    }
}


