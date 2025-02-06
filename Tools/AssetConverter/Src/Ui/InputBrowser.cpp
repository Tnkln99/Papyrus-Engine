#include "InputBrowser.h"

#include <utility>

#include "imgui.h"
#include "Nmd/FileHelper.h"
#include "Nmd/Logger.h"


namespace Ui
{
    InputBrowser::InputBrowser(std::string  inputFolderPath) : m_inputFolderPath(std::move(inputFolderPath))
    {
        scanFolder();
    }


    void InputBrowser::scanFolder()
    {
        m_files.clear();

        if (!Nmd::FileHelper::fileExists(m_inputFolderPath)) {
            NOMAD_LOG(Nmd::LogLevel::Error, "Folder does not exist: {}", m_inputFolderPath);
            return;
        }

        NOMAD_LOG(Nmd::LogLevel::Info, "Scanning folder: {}", m_inputFolderPath);

        m_files = Nmd::FileHelper::listFiles(m_inputFolderPath);
    }

    void InputBrowser::render() {
        ImGui::Begin("Input Browser");

        ImGui::Text("Folder: %s", m_inputFolderPath.c_str());
        ImGui::Separator();

        for (const auto& file : m_files) {
            bool isSelected = (std::ranges::find(m_selectedFiles, file) != m_selectedFiles.end());

            if (ImGui::Selectable(file.c_str(), isSelected)) {
                if (isSelected) {
                    std::erase(m_selectedFiles, file);
                    NOMAD_LOG(Nmd::LogLevel::Message, "Deselected file: {}", file);
                } else {
                    m_selectedFiles.push_back(file);
                    NOMAD_LOG(Nmd::LogLevel::Message, "Selected file: {}", file);
                }
            }
        }

        ImGui::End();
    }

}
