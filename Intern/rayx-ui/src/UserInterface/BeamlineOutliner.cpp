#include "BeamlineOutliner.h"

#include <imgui.h>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Settings.h"

// Constructor / Destructor
BeamlineOutliner::BeamlineOutliner() {}
BeamlineOutliner::~BeamlineOutliner() {}

// Recursive helper: render a Group and its children as an ImGui tree.
void BeamlineOutliner::renderImGuiTreeFromGroup(RAYX::Group* group, RAYX::BeamlineNode*& selected, int depth) {
    if (!group) return;
    int ctr = 0;
    for (auto& child : *group) {
        RAYX::NodeType nodeType = RAYX::getNodeType(child);
        std::string label;

        // Use the node's name if possible.
        switch (nodeType) {
            case RAYX::NodeType::OpticalElement:
                label = std::get<std::unique_ptr<RAYX::DesignElement>>(child)->getName();
                break;
            case RAYX::NodeType::LightSource:
                label = std::get<std::unique_ptr<RAYX::DesignSource>>(child)->getName();
                break;
            case RAYX::NodeType::Group:
                label = "Group";
                break;
        }

        // Create a unique label ID for ImGui (so nodes with the same name are distinguished).
        // ? Why is this needed?
        // TODO: Either enforce unique names in DesignEl/Src or display the correct name in UI
        std::stringstream ss;
        ss << label << "##" << depth << "_" << ctr++;
        std::string uniqueLabel = ss.str();

        // Set tree node flags: if the node is a leaf (not a group), mark it as such.
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        bool isLeaf = (nodeType != RAYX::NodeType::Group);
        if (isLeaf) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }
        bool nodeOpen = ImGui::TreeNodeEx(uniqueLabel.c_str(), flags, "%s", label.c_str());

        // If the user clicks the node, you can update selection info here.
        if (ImGui::IsItemClicked()) {
            selected = &child;
        }

        // If this node is a Group and is open, recursively render its children.
        if (nodeOpen && !isLeaf) {
            // Get the group pointer from the variant.
            const auto& childGroupPtr = std::get<std::unique_ptr<RAYX::Group>>(child);
            // Dereference it to call the function.
            renderImGuiTreeFromGroup(childGroupPtr.get(), selected, depth + 1);
            ImGui::TreePop();
        }
    }
}

// This function displays the beamline outline window using the existing data structure.
void BeamlineOutliner::showBeamlineOutlineWindow(UIBeamlineInfo& blInfo) {
    ImGui::Begin("Beamline Outline");

    if (blInfo.beamline == nullptr) {
        ImGui::Text("Beamline not loaded.");
    } else {
        // Render the tree starting from the beamline root.
        renderImGuiTreeFromGroup(blInfo.beamline, blInfo.selectedNode, 0);
    }

    ImGui::End();
}
