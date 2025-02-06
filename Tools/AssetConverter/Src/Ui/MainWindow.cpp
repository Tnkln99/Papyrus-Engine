#include "MainWindow.h"

#include "imgui.h"

namespace Ui
{
    MainWindow::MainWindow(const std::string& inputFolderPath, const std::string& outputFolderPath)
    : m_assetBrowser(inputFolderPath), m_outputBrowser(outputFolderPath) {}

        MainWindow::~MainWindow() = default;

        void MainWindow::render(const int width, const int height)
        {
            const ImGuiIO& io = ImGui::GetIO();

            // Set the next window position and size to match the display size every frame.
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            const auto displaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
            ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);

            // Optional: Set window flags to remove decoration and prevent manual resize or movement.
            constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                                      ImGuiWindowFlags_NoResize   |
                                                      ImGuiWindowFlags_NoMove     |
                                                      ImGuiWindowFlags_NoCollapse;

            // Begin the window. The window will automatically resize based on the display size.
            ImGui::Begin("Auto-Resizing Window", nullptr, window_flags);
            ImGui::End();

            m_assetBrowser.render();
            m_outputBrowser.render();
            m_convertOptions.render();
            m_logger.render();
    }
}
