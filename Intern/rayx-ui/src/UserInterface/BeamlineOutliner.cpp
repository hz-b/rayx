#include "BeamlineOutliner.h"

#include <imgui.h>

#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Settings.h"

// Constructor / Destructor
BeamlineOutliner::BeamlineOutliner() {}
BeamlineOutliner::~BeamlineOutliner() {}

// Recursive helper: render a Group and its children as an ImGui tree.
void BeamlineOutliner::renderImGuiTreeFromGroup(rayx::Group* group, rayx::BeamlineNode*& selected, CameraController& cam, int depth) {
    if (!group) return;
    int ctr = 0;
    for (auto& child : *group) {
        std::string label;

        // Use the node's name if possible.
        if (child->isElement()) {
            label = static_cast<rayx::DesignElement*>(child.get())->getName();
        } else if (child->isSource()) {
            label = static_cast<rayx::DesignSource*>(child.get())->getName();
        } else if (child->isGroup()) {
            label = "Group";
        }

        std::string buttonLabel = "<-##" + std::to_string(ctr++);
        if (ImGui::Button(buttonLabel.c_str())) {
            const glm::dvec4& pos = child->getWorldPosition();
            cam.lookAtPoint({pos.x, pos.y, pos.z});
        }

        // Set tree node flags: if the node is a leaf (not a group), mark it as such.
        ImGui::SameLine();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (!child->isGroup()) { flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; }
        bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags, "%s", label.c_str());

        if (ImGui::IsItemClicked()) { selected = child.get(); }

        // If this node is a Group and is open, recursively render its children.
        if (nodeOpen && child->isGroup()) {
            auto* childGroupPtr = static_cast<rayx::Group*>(child.get());
            renderImGuiTreeFromGroup(childGroupPtr, selected, cam, depth + 1);
            ImGui::TreePop();
        }
    }
}

// This function displays the beamline outline window using the existing data structure.
void BeamlineOutliner::showBeamlineOutlineWindow(UIParameters& uiParams) {
    ImGui::Begin("Beamline Outline");
    UIBeamlineInfo& blInfo = uiParams.beamlineInfo;
    if (blInfo.beamline == nullptr) {
        ImGui::Text("Beamline not loaded.");
    } else {
        // Render the tree starting from the beamline root.
        renderImGuiTreeFromGroup(blInfo.beamline, blInfo.selectedNode, uiParams.camController, 0);
    }

    ImGui::End();
}
