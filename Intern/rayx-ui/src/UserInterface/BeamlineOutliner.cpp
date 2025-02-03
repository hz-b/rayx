#include "BeamlineOutliner.h"

#include <imgui.h>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"

// Constructor / Destructor
BeamlineOutliner::BeamlineOutliner() {}
BeamlineOutliner::~BeamlineOutliner() {}

// Set the beamline to be outlined.
void BeamlineOutliner::setBeamline(const RAYX::Beamline* beamline) { m_Beamline = beamline; }

// Recursive helper: render a Group and its children as an ImGui tree.
void BeamlineOutliner::renderImGuiTreeFromGroup(const RAYX::Group& group, int depth) {
    // For each child in the group...
    const auto& children = group.getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        const RAYX::BeamlineNode& node = children[i];
        // Determine node type via the provided utility function.
        RAYX::NodeType nodeType = RAYX::getNodeType(node);
        std::string label;

        // Use the node's name if possible.
        switch (nodeType) {
            case RAYX::NodeType::OpticalElement:
                label = std::get<std::shared_ptr<RAYX::DesignElement>>(node)->getName();
                break;
            case RAYX::NodeType::LightSource:
                label = std::get<std::shared_ptr<RAYX::DesignSource>>(node)->getName();
                break;
            case RAYX::NodeType::Group:
                // If Group doesn’t have a name member, you can hard-code one or add one.
                label = "Group";
                break;
        }

        // Create a unique label ID for ImGui (so nodes with the same name are distinguished).
        std::stringstream ss;
        ss << label << "##" << depth << "_" << i;
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
            // For example, update a UIBeamlineInfo or call a callback.
            // (This part depends on how you manage selections.)
        }

        // If this node is a Group and is open, recursively render its children.
        if (nodeOpen && !isLeaf) {
            // Get the group pointer from the variant.
            const auto& childGroupPtr = std::get<std::shared_ptr<RAYX::Group>>(node);
            // Dereference it to call the function.
            renderImGuiTreeFromGroup(*childGroupPtr, depth + 1);
            ImGui::TreePop();
        }
    }
}

// This function displays the beamline outline window using the existing data structure.
void BeamlineOutliner::showBeamlineOutlineWindow() {
    ImGui::Begin("Beamline Outline");

    if (m_Beamline == nullptr) {
        ImGui::Text("Beamline not loaded.");
    } else {
        // Render the tree starting from the beamline root.
        renderImGuiTreeFromGroup(*m_Beamline, 0);
    }

    ImGui::End();
}
