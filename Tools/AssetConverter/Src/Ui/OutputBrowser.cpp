#include "OutputBrowser.h"

#include "imgui.h"
#include "Nmd/FileHelper.h"
#include "Nmd/Logger.h"


namespace Ui
{
    OutputBrowser::OutputBrowser(const std::string &outputFolderPath) : m_outputFolderPath(outputFolderPath) {}

    void OutputBrowser::render()
    {
        ImGui::Begin("Output browser");

        ImGui::Text("Folder: %s", m_outputFolderPath.c_str());
        ImGui::Separator();

        for (const auto& file : m_files) {
            ImGui::Text("%s", file.c_str());
        }

        ImGui::End();
    }

    void OutputBrowser::scanFolder()
    {
        if (Nmd::FileHelper::fileExists(m_outputFolderPath))
        {
            m_files.clear();
            m_files = Nmd::FileHelper::listFiles(m_outputFolderPath);
        }
    }
}
