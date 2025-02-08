#include "Logger.h"
#include "Nmd/Logger.h"

#include "imgui.h"

namespace Ui
{
    Logger::Logger() {}

    void Logger::render()
    {
        ImGui::Begin("Logger", nullptr, ImGuiWindowFlags_NoMove);
        // Optional menu bar for log options (e.g., clear log)
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Clear"))
                {
                    Nmd::Logger::clearLogString();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Create a scrolling child region that fills the remaining space in the window.
        // The flags force the vertical (and horizontal) scrollbar to appear when necessary.
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true,
                            ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_HorizontalScrollbar);

        // Retrieve the accumulated log text
        const std::string& logs = Nmd::Logger::getLogString();

        // Display the log text without formatting overhead
        ImGui::TextUnformatted(logs.c_str());

        // Optional auto-scroll: if already at the bottom, keep the view pinned to the latest log entry
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::End();
    }
}
