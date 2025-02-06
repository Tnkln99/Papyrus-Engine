#pragma once
#include <string>
#include <vector>

namespace Ui
{
    class InputBrowser
    {
    public:
        explicit InputBrowser(std::string inputFolderPath);

        void render();

        // Delete copy constructor and copy assignment operator
        InputBrowser(const InputBrowser&) = delete;
        InputBrowser& operator=(const InputBrowser&) = delete;

        // Delete move constructor and move assignment operator
        InputBrowser(InputBrowser&&) = delete;
        InputBrowser& operator=(InputBrowser&&) = delete;

    private:
        void scanFolder();

    private:
        std::string m_inputFolderPath;
        std::vector<std::string> m_files;
        std::vector<std::string> m_selectedFiles;
    };
}



