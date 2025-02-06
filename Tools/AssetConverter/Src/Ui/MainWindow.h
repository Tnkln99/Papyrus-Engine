#pragma once

#include "ConvertOptions.h"
#include "InputBrowser.h"
#include "Logger.h"
#include "OutputBrowser.h"

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
        InputBrowser m_assetBrowser;
        OutputBrowser m_outputBrowser;
        ConvertOptions m_convertOptions;
        Logger m_logger;
    };
}

