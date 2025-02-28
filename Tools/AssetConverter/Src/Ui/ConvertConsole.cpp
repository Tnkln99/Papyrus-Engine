#include "ConvertConsole.h"

#include "imgui.h"

namespace Ui
{
    ConvertConsole::ConvertConsole()
        : m_selectedOption(ConvertOption::StaticModel) // Default option
    {
    }

    void ConvertConsole::render()
    {
        if (ImGui::Begin("Convert Console", nullptr, ImGuiWindowFlags_NoMove))
        {
            // Start a group for the radio buttons so we can measure their combined bounding box
            ImGui::BeginGroup();
            {
                if (ImGui::RadioButton("Static Model", m_selectedOption == ConvertOption::StaticModel))
                {
                    m_selectedOption = ConvertOption::StaticModel;
                }
                if (ImGui::RadioButton("Clustered Static Model", m_selectedOption == ConvertOption::ClusteredModel))
                {
                    m_selectedOption = ConvertOption::ClusteredModel;
                }
                if (ImGui::RadioButton("Cloth", m_selectedOption == ConvertOption::Cloth))
                {
                    m_selectedOption = ConvertOption::Cloth;
                }
            }
            // End the group and store its bounding box size
            ImGui::EndGroup();

            // Retrieve size of the group we just drew
            const ImVec2 radioGroupSize = ImGui::GetItemRectSize();

            // Place the "Convert" button on the same line (to the right)
            ImGui::SameLine();

            // Create a square button matching the total height of the radio button group
            const ImVec2 buttonSize(radioGroupSize.y, radioGroupSize.y);
            if (ImGui::Button("Convert\nselected\ninput", buttonSize))
            {
                m_bConvertButtonClicked = true;
            }
            else
            {
                m_bConvertButtonClicked = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Import\ninput\nfrom\ncomputer", buttonSize))
            {
                m_bFileExplorerButtonClicked = true;
            }
            else
            {
                m_bFileExplorerButtonClicked = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete\nselected\ninput", buttonSize))
            {
                m_bDeleteInputClicked = true;
            }
            else
            {
                m_bDeleteInputClicked = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete\nselected\noutput", buttonSize))
            {
                m_bDeleteOutputClicked = true;
            }
            else
            {
                m_bDeleteOutputClicked = false;
            }
        }
        ImGui::End();
    }

    bool ConvertConsole::startConversion() const
    {
        return m_bConvertButtonClicked;
    }

    bool ConvertConsole::showFileExplorer() const
    {
        return m_bFileExplorerButtonClicked;
    }

    bool ConvertConsole::deleteInputSelected() const
    {
        return m_bDeleteInputClicked;
    }

    bool ConvertConsole::deleteOutputSelected() const
    {
        return m_bDeleteOutputClicked;
    }
}
