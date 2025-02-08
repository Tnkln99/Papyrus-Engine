#include "Nmd/FileHelper.h"
#include "Nmd/Logger.h"

#include <filesystem>

#ifdef _WIN32
#include <Shlobj.h>   // For SHGetKnownFolderPath
#include <KnownFolders.h> // For FOLDERID_Downloads
#include <string>
#endif

namespace fs = std::filesystem;

namespace Nmd
{
    std::vector<std::string> FileHelper::listFilesWithExtension(const std::string& directory, const std::string& extension)
    {
        std::vector<std::string> files;
        try
        {
            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file() && entry.path().extension() == extension)
                {
                    files.push_back(entry.path().string());
                }
            }
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error listing files: {}", e.what());
        }
        return files;
    }

    std::vector<std::string> FileHelper::listFilesWithExtensions(const std::string& directory, const std::vector<std::string>& extensions)
    {
        std::vector<std::string> files;
        try
        {
            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file())
                {
                    // Obtain the file extension
                    std::string fileExtension = entry.path().extension().string();

                    // Check if this extension is in the provided list
                    if (std::ranges::find(extensions, fileExtension) != extensions.end())
                    {
                        files.push_back(entry.path().string());
                    }
                }
            }
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
            for (const auto& entry : fs::recursive_directory_iterator(directory))
            {
                if (entry.is_regular_file() && entry.path().extension() == extension)
                {
                    files.push_back(entry.path().string());
                }
            }
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
            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_regular_file())
                {
                    files.push_back(entry.path().string());
                }
            }
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
        return exists;
    }

    bool FileHelper::deleteFile(const std::string& filePath)
    {
        try
        {
            if (fs::remove(filePath))
            {
                return true;
            }
        }
        catch (const std::exception& e)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error deleting file '{}': {}", filePath, e.what());
        }
        return false;
    }

    bool FileHelper::deleteFiles(const std::vector<std::string>& filePaths)
    {
        bool allDeleted = true;
        for (const auto& filePath : filePaths)
        {
            // Use the existing deleteFile method for consistency
            if (!deleteFile(filePath))
            {
                allDeleted = false;
            }
        }
        return allDeleted;
    }

    uintmax_t FileHelper::getFileSize(const std::string& filePath)
    {
        try
        {
            if (fs::exists(filePath) && fs::is_regular_file(filePath))
            {
                uintmax_t size = fs::file_size(filePath);
                return size;
            }
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
                return true;
            }
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

    std::string FileHelper::getFileName(const std::string &filePath)
    {
        const std::filesystem::path pathObj(filePath);
        return pathObj.filename().string();
    }

    std::vector<std::string> FileHelper::listDirectories(const std::string &directory)
    {
        std::vector<std::string> result;
        try
        {
            for (auto& entry : fs::directory_iterator(directory))
            {
                if (entry.is_directory())
                {
                    result.push_back(entry.path().string());
                }
            }
        }
        catch (...)
        {
            NOMAD_LOG(Nmd::LogLevel::Error, "Error listing directories: {}", directory);
        }
        return result;
    }

    std::vector<std::string> FileHelper::listAllDrives()
    {
        std::vector<std::string> drives;

#ifdef _WIN32
        // Check drive letters from A: to Z:
        // If valid, add "X:\"
        for (char drive = 'A'; drive <= 'Z'; ++drive)
        {
            std::string path = std::string(1, drive) + ":\\";
            if (fs::exists(path))
            {
                drives.push_back(path);
            }
        }
#else
        // Example for Linux/macOS: single root.
        drives.push_back("/");
#endif
        return drives;
    }

    std::string FileHelper::getDownloadsFolder()
    {
#ifdef _WIN32
        // Windows approach from above
        PWSTR widePath = nullptr;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &widePath);
        if (SUCCEEDED(hr))
        {
            char narrowPath[MAX_PATH];
            wcstombs(narrowPath, widePath, MAX_PATH);
            CoTaskMemFree(widePath);
            return std::string(narrowPath);
        }
        // Fallback if Windows call fails:
        return "";
#else
        // On Linux/macOS, try ~/Downloads
        const char* homeDir = std::getenv("HOME");
        if (homeDir)
        {
            std::string downloads = std::string(homeDir) + "/Downloads";
            return downloads;
        }
        // If HOME not set, just fallback to current directory or "/"
        return ".";
#endif
    }

    std::string FileHelper::normalizePath(std::string &path)
    {
        try
        {
            fs::path p(path);
            p = weakly_canonical(p);
            return p.string();
        }
        catch (...)
        {
            return path;
        }
    }

    bool FileHelper::isPathInside(const std::string &parentPath, const std::string &childPath)
    {
        const fs::path nPath(parentPath);
        const fs::path tPath(childPath);
        // if the target's string doesn't contain the node path, skip:
        if (tPath.native().find(nPath.native()) == std::string::npos)
        {
            return false;
        }

        return true;
    }

    bool FileHelper::moveFile(const std::string& sourcePath, const std::string& destinationFolder)
    {
        if (!fileExists(sourcePath))
        {
            return false;
        }

        try
        {
            // Construct the destination path using std::filesystem::path
            fs::path srcPath(sourcePath);
            fs::path destFolder(destinationFolder);
            fs::path destPath = destFolder / srcPath.filename(); // Append filename to folder

            // Try to rename (works if on the same volume)
            fs::rename(srcPath, destPath);
            return true;
        }
        catch (const std::exception&) {
            // If rename fails (e.g., different volumes), do a copy & remove fallback
            try {
                fs::copy_file(sourcePath, destinationFolder / fs::path(sourcePath).filename(), fs::copy_options::overwrite_existing);
                fs::remove(sourcePath);
                return true;
            }
            catch (const std::exception&) {
                return false;
            }
        }
    }

    std::string FileHelper::constructFilePathWithExtension(const std::string& dirName, const std::string& fileName, const std::string& extension)
    {
        std::filesystem::path pathDir(dirName);
        pathDir /= fileName;
        pathDir.replace_extension(extension);
        return pathDir.string();
    }
}


