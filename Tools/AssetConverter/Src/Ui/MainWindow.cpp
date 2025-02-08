#include "MainWindow.h"

#include "FileExplorer.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Mrc/Exporter.h"
#include "Mrc/Importer.h"
#include "Nmd/FileHelper.h"
#include "Nmd/Logger.h"

namespace Ui
{
    MainWindow::MainWindow(const std::string& inputFolderPath, const std::string& outputFolderPath)
    : m_inputBrowser("Input Browser", inputFolderPath), m_outputBrowser("Output Browser", outputFolderPath)
    {
    }

        MainWindow::~MainWindow() = default;

        void MainWindow::render(const int width, const int height)
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            const auto displaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
            ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);

            constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                                      ImGuiWindowFlags_NoResize   |
                                                      ImGuiWindowFlags_NoMove     |
                                                      ImGuiWindowFlags_NoCollapse |
                                                      ImGuiDockNodeFlags_NoResize;

            ImGui::Begin("Auto-Resizing Window", nullptr, window_flags);
            ImGui::End();

            m_inputBrowser.render();
            m_outputBrowser.render();
            m_convertConsole.render();
            m_logger.render();

            if (m_convertConsole.startConversion()) // button clicked
            {
                const auto& selectedFiles = m_inputBrowser.getSelectedFiles();
                if (!selectedFiles.empty())
                {
                    for (const std::string& file : selectedFiles)
                    {
                        NOMAD_LOG(Nmd::LogLevel::Info, "Converting file {}", file);
                        Mrc::Importer importer(file);

                        Mrc::AStaticModel model;
                        importer.getStaticModel(model);

                        std::string outputName = Nmd::FileHelper::getFileName(file);

                        Mrc::Exporter::exportStaticModel(model, "Output", outputName);

                        m_outputBrowser.refresh();
                    }
                }
            }

            if (m_convertConsole.showFileExplorer())
            {
                m_bOpenFileExplorer = true;
                ImGui::OpenPopup("File Explorer");
            }

            if (m_convertConsole.deleteInputSelected())
            {
                m_inputBrowser.deleteSelectedFiles();
            }

            if (m_convertConsole.deleteOutputSelected())
            {
                m_outputBrowser.deleteSelectedFiles();
            }

            m_fileExplorer.showFileExplorerPopup(&m_bOpenFileExplorer);
            if (m_fileExplorer.isSelectionMade())
            {
                if (!Nmd::FileHelper::moveFile(m_fileExplorer.getSelectedFileName(), "Input"))
                {
                    NOMAD_LOG(Nmd::LogLevel::Info, "Could not copy the file");
                }
                m_fileExplorer.setSelectionMade(false);
                m_inputBrowser.refresh();
            }
    }
}
