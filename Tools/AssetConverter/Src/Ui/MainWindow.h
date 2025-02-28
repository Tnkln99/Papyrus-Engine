#pragma once

#include "ConvertConsole.h"
#include "FileExplorer.h"
#include "SimpleBrowser.h"
#include "Logger.h"

namespace Ui
{
    class MainWindow
    {
    public:
        explicit MainWindow(const std::string& inputFolderPath, const std::string& outputFolderPath);
        ~MainWindow();

        void render(int width, int height);

        // Delete copy constructor and copy assignment operator
        MainWindow(const MainWindow&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;

        // Delete move constructor and move assignment operator
        MainWindow(MainWindow&&) = delete;
        MainWindow& operator=(MainWindow&&) = delete;
    private:
        bool m_bOpenFileExplorer = false;
        bool m_bOpenClusteredStaticModelConvertOptions = false;

        FileExplorer m_fileExplorer;

        SimpleBrowser m_inputBrowser;
        SimpleBrowser m_outputBrowser;
        ConvertConsole m_convertConsole;
        Logger m_logger;
    };
}

