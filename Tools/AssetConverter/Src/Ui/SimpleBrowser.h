#pragma once
#include <string>
#include <vector>

namespace Ui
{
    class SimpleBrowser
    {
    public:
        SimpleBrowser(std::string  name, std::string  folderPath);

        void render();
        void deleteSelectedFiles();
        void refresh();

        const std::vector<std::string>& getSelectedFiles()
        {
            return m_selectedFiles;
        }

        // Delete copy constructor and copy assignment operator
        SimpleBrowser(const SimpleBrowser&) = delete;
        SimpleBrowser& operator=(const SimpleBrowser&) = delete;

        // Delete move constructor and move assignment operator
        SimpleBrowser(SimpleBrowser&&) = delete;
        SimpleBrowser& operator=(SimpleBrowser&&) = delete;

    private:
        void scanFolder();

    private:
        std::string m_folderPath;
        std::vector<std::string> m_files;
        std::vector<std::string> m_selectedFiles;

        std::string m_name;
    };
}
