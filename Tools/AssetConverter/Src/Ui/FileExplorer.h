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

        // after using isSelectionMade it is the users responsibility to implement the logic and switch off the selection made
        void endSelectionMade()
        {
            m_bSelectionMade = false;
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
