#pragma once

#include <string>
#include <vector>

namespace Ui
{
    class FileExplorer {
    public:
        void showFileExplorerPopup(bool* open);

        [[nodiscard]] bool isSelectionMade() const
        {
            return m_bSelectionMade;
        }

        void setSelectionMade(const bool isSelectionMade)
        {
            m_bSelectionMade = isSelectionMade;
        }

        [[nodiscard]] std::string getSelectedFileName() const
        {
            return m_selectedFile;
        }

    private:
        const std::vector<std::string> m_cExtensionsList = {".fbx", ".obj"};
        bool m_bSelectionMade = false;
        std::string m_selectedFile;
    };
}
