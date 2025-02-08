#include "SimpleBrowser.h"

#include <unordered_set>
#include <utility>

#include "imgui.h"
#include "Nmd/FileHelper.h"
#include "Nmd/Logger.h"


namespace Ui
{
    SimpleBrowser::SimpleBrowser(std::string  name, std::string  folderPath) : m_folderPath(std::move(folderPath)), m_name(std::move(name))
    {
        scanFolder();
    }

    void SimpleBrowser::render() {
        ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_NoMove);

        ImGui::Text("Folder: %s", m_folderPath.c_str());
        ImGui::Separator();

        for (const auto& file : m_files) {
            const bool isSelected = (std::ranges::find(m_selectedFiles, file) != m_selectedFiles.end());

            if (ImGui::Selectable(file.c_str(), isSelected)) {
                if (isSelected)
                {
                    NOMAD_LOG(Nmd::LogLevel::Info, "File deselected : {}", file);
                    std::erase(m_selectedFiles, file);
                }
                else
                {
                    NOMAD_LOG(Nmd::LogLevel::Info, "File selected : {}", file);
                    m_selectedFiles.push_back(file);
                }
            }
        }

        ImGui::End();
    }

    void SimpleBrowser::deleteSelectedFiles()
    {
        Nmd::FileHelper::deleteFiles(m_selectedFiles);
        // Put all selected files in a hash set for quick lookup
        std::unordered_set selectedSet(m_selectedFiles.begin(), m_selectedFiles.end());

        // Use remove_if to partition out matching files
        const auto newEnd = std::remove_if(m_files.begin(), m_files.end(),
            [&](const std::string& file)
            {
                // Return true if file is in the set of selected files
                return selectedSet.contains(file);
            }
        );

        // Erase the 'removed' part of the vector
        m_files.erase(newEnd, m_files.end());
        m_selectedFiles.clear();
        refresh();
    }

    void SimpleBrowser::refresh()
    {
        scanFolder();
    }

    void SimpleBrowser::scanFolder()
    {
        m_files.clear();

        if (!Nmd::FileHelper::fileExists(m_folderPath)) {
            NOMAD_LOG(Nmd::LogLevel::Error, "Folder does not exist: {}", m_folderPath);
            return;
        }

        m_files = Nmd::FileHelper::listFiles(m_folderPath);
    }
}
