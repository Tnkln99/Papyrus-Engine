#include "ConvertOptions.h"

#include "imgui.h"

namespace Ui
{
    ConvertOptions::ConvertOptions()
        : m_selectedOption(ConvertOption::StaticModel) // Default option
    {
    }

    void ConvertOptions::render()
    {
        if (ImGui::Begin("Convert Options"))
        {
            if (ImGui::RadioButton("Static Model", m_selectedOption == ConvertOption::StaticModel))
            {
                m_selectedOption = ConvertOption::StaticModel;
            }
            if (ImGui::RadioButton("Meshlet Model", m_selectedOption == ConvertOption::MeshletModel))
            {
                m_selectedOption = ConvertOption::MeshletModel;
            }
            if (ImGui::RadioButton("Cloth", m_selectedOption == ConvertOption::Cloth))
            {
                m_selectedOption = ConvertOption::Cloth;
            }
        }
        ImGui::End();
    }
}
