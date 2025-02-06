#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

namespace Nmd
{
    class FileHelper {
    public:
        static std::vector<std::string> listFilesWithExtension(const std::string& directory, const std::string& extension);
        static std::vector<std::string> listFilesRecursiveWithExtension(const std::string& directory, const std::string& extension);
        static std::vector<std::string> listFiles(const std::string& directory);
        static bool fileExists(const std::string& filePath);
        static bool deleteFile(const std::string& filePath);
        static uintmax_t getFileSize(const std::string& filePath);
        static bool createDirectory(const std::string& dirPath);
        static std::string getFileExtension(const std::string& fileName);
    };
}





