#pragma once

#include <string>
#include <vector>

namespace Ui
{
    class OutputBrowser
    {
    public:
        explicit OutputBrowser(const std::string &outputFolderPath);

        void render();

        // Delete copy constructor and copy assignment operator
        OutputBrowser(const OutputBrowser&) = delete;
        OutputBrowser& operator=(const OutputBrowser&) = delete;

        // Delete move constructor and move assignment operator
        OutputBrowser(OutputBrowser&&) = delete;
        OutputBrowser& operator=(OutputBrowser&&) = delete;

    private:
        void scanFolder();

    private:
        std::string m_outputFolderPath;
        std::vector<std::string> m_files;
    };
}



