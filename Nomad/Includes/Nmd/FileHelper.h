#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace Nmd
{
    class FileHelper {
    public:
        static std::vector<std::string> listFilesWithExtension(const std::string& directory, const std::string& extension);
        static std::vector<std::string> listFilesWithExtensions(const std::string& directory, const std::vector<std::string>& extensions);
        static std::vector<std::string> listFilesRecursiveWithExtension(const std::string& directory, const std::string& extension);
        static std::vector<std::string> listFiles(const std::string& directory);

        static bool fileExists(const std::string& filePath);

        static bool deleteFile(const std::string& filePath);
        static bool deleteFiles(const std::vector<std::string> &filePaths);

        static uintmax_t getFileSize(const std::string& filePath);

        static bool createDirectory(const std::string& dirPath);

        static std::string getFileExtension(const std::string& fileName);

        static std::string getFileName(const std::string& filePath);

        static std::vector<std::string> listDirectories(const std::string& directory);

        static std::vector<std::string> listAllDrives();

        static std::string getDownloadsFolder();

        static std::string normalizePath(std::string& path);

        static bool isPathInside(const std::string& parentPath, const std::string& childPath);

        static bool moveFile(const std::string &sourcePath, const std::string &destinationPath);

        static std::string constructFilePathWithExtension(const std::string& dirName, const std::string& fileName, const std::string& extension);
    };
}





