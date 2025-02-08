#include "FileExplorer.h"

#include "Nmd/FileHelper.h"

#include <string>
#include <vector>
#include <imgui.h>


namespace Ui
{
    struct FolderNode
    {
        std::string m_path;
        std::string m_name;
        bool m_bChildrenLoaded = false;
        bool m_bShouldOpen = false;
        std::vector<FolderNode> m_children;
    };


    //--------------------------------------------------------------------------------------
    // Populate children of a FolderNode by listing its immediate subdirectories
    //--------------------------------------------------------------------------------------
    void populateFolderNodeChildren(FolderNode& node)
    {
        if (node.m_bChildrenLoaded)
            return;

        node.m_children.clear();

        std::vector<std::string> subDirs = Nmd::FileHelper::listDirectories(node.m_path);
        for (auto& dirPath : subDirs)
        {
            FolderNode child;
            child.m_path = Nmd::FileHelper::normalizePath(dirPath);
            child.m_name = Nmd::FileHelper::getFileName(dirPath);
            if (child.m_name.empty())
                child.m_name = child.m_path;

            child.m_bChildrenLoaded = false;
            child.m_bShouldOpen = false; // default false
            node.m_children.push_back(child);
        }
        node.m_bChildrenLoaded = true;
    }

    //--------------------------------------------------------------------------------------
    // Recursively expands 'node' until we find 'targetPath'; each ancestor sets m_bShouldOpen
    // so ImGui will auto-expand it. Returns true if found, else false.
    //--------------------------------------------------------------------------------------
    bool expandFolderNodeToPath(FolderNode& node,
                                       const std::string& targetPath,
                                       std::string& selectedFolder)
    {
        // If this node *is* the target folder, we've arrived
        if (node.m_path == targetPath)
        {
            node.m_bShouldOpen = true;  // open the final node
            selectedFolder = targetPath;
            return true;
        }

        if (!Nmd::FileHelper::isPathInside(node.m_path, targetPath))
        {
            return false;
        }

        // Expand children
        populateFolderNodeChildren(node);

        // Recur on children
        for (auto& child : node.m_children)
        {
            if (expandFolderNodeToPath(child, targetPath, selectedFolder))
            {
                // If child is in the path, mark this node open as well
                node.m_bShouldOpen = true;
                return true;
            }
        }
        return false;
    }

    void expandTreeToPath(std::vector<FolderNode>& rootNodes,
                                 const std::string& targetPath,
                                 std::string& selectedFolder)
    {
        for (auto& root : rootNodes)
        {
            if (expandFolderNodeToPath(root, targetPath, selectedFolder))
            {
                break;
            }
        }
    }

    void drawFolderTree(FolderNode& node, std::string& selectedFolder)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;

        // Highlight the selected folder
        if (node.m_path == selectedFolder)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        // If this node is on the path to Downloads (or another target), we default-open it
        if (node.m_bShouldOpen)
        {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        bool isOpen = ImGui::TreeNodeEx(node.m_name.c_str(), flags);

        // If user clicks (not toggling arrow), select
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedFolder = node.m_path;
        }

        if (isOpen)
        {
            if (ImGui::IsItemToggledOpen())
            {
                populateFolderNodeChildren(node);
            }
            for (auto& child : node.m_children)
            {
                drawFolderTree(child, selectedFolder);
            }
            ImGui::TreePop();
        }
    }

    //--------------------------------------------------------------------------------------
    // The main popup function
    //--------------------------------------------------------------------------------------
    void FileExplorer::showFileExplorerPopup(bool* open)
    {
        ImGui::SetNextWindowSize(ImVec2(700.0f, 500.0f), ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("File Explorer", open, ImGuiWindowFlags_NoResize))
        {
            // static data so it persists
            static bool s_bRootDrivesLoaded = false;
            static std::vector<FolderNode> s_rootNodes;
            static std::string s_selectedFolder;
            static char s_fileSearchFilter[256] = { 0 };

            if (!s_bRootDrivesLoaded)
            {
                s_bRootDrivesLoaded = true;
                s_rootNodes.clear();

                // 1) Gather root drives
                auto drives = Nmd::FileHelper::listAllDrives();
                for (auto& d : drives)
                {
                    FolderNode root;
                    root.m_path = Nmd::FileHelper::normalizePath(d);
                    root.m_name = root.m_path;
                    root.m_bChildrenLoaded = false;
                    root.m_bShouldOpen = false;
                    s_rootNodes.push_back(root);
                }

                // 2) Attempt to get the Downloads path
                std::string downloadFolder = Nmd::FileHelper::getDownloadsFolder();
                const std::string downloadsPath = Nmd::FileHelper::normalizePath(downloadFolder);
                if (!downloadsPath.empty())
                {
                    // We'll set the selected folder to downloads
                    s_selectedFolder = downloadsPath;

                    // Expand the tree so it's visible on the left
                    expandTreeToPath(s_rootNodes, downloadsPath, s_selectedFolder);
                }
                else
                {
                    // Fallback: if no downloads path, pick the first drive
                    if (!s_rootNodes.empty())
                    {
                        s_selectedFolder = s_rootNodes[0].m_path;
                    }
                }
            }

            // Layout: 2 columns
            ImGui::Columns(2, nullptr, true);

            // Left: Folder tree
            ImGui::BeginChild("##FolderTree", ImVec2(0, 0), true);
            {
                for (auto& root : s_rootNodes)
                {
                    drawFolderTree(root, s_selectedFolder);
                }
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            // Right: File listing + filter
            ImGui::BeginChild("##FileList", ImVec2(0, -40), true);
            {
                ImGui::Text("Search:");
                ImGui::SameLine();
                ImGui::InputText("##fileSearch", s_fileSearchFilter, IM_ARRAYSIZE(s_fileSearchFilter));

                ImGui::Separator();

                // List files in the selected folder
                if (!s_selectedFolder.empty())
                {
                    const auto files = Nmd::FileHelper::listFilesWithExtensions(s_selectedFolder, m_cExtensionsList);
                    for (auto& f : files)
                    {
                        std::string fileName = Nmd::FileHelper::getFileName(f);
                        if (fileName.find(s_fileSearchFilter) == std::string::npos)
                            continue;

                        const bool isSel = (f == m_selectedFile);
                        if (ImGui::Selectable(fileName.c_str(), isSel))
                        {
                            m_selectedFile = f;
                        }
                    }
                }
            }
            ImGui::EndChild();

            // OK / Cancel
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30);

            if (ImGui::Button("OK"))
            {
                m_bSelectionMade = true;
                ImGui::CloseCurrentPopup();
                *open = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                m_selectedFile.clear();
                ImGui::CloseCurrentPopup();
                *open = false;
            }

            ImGui::EndPopup();
        }

        //m_selectedFile.clear();
    }
} // namespace Ui
