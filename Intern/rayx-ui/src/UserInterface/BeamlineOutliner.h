#pragma once

#include <glm.h>

#include <memory>
#include <string>
#include <vector>

#include "UserInterface/Settings.h"

// Simple TreeNode
struct TreeNode {
    std::string name;
    std::string type;
    SelectedType category = SelectedType::None;
    int index = -1;
    std::vector<TreeNode> children;

    TreeNode(const std::string& name, const std::string& type = "", SelectedType category = SelectedType::None)
        : name(name), type(type), category(category) {}
};

class BeamlineOutliner {
  public:
    BeamlineOutliner();
    ~BeamlineOutliner();

    void showBeamlineOutlineWindow(UIParameters& uiParams);

  private:
    std::unique_ptr<TreeNode> m_pTreeRoot = nullptr;

    int m_lightSourceIndex = 0;
    int m_opticalElementIndex = 0;
    std::filesystem::path m_currentRML;

    void renderImGuiTree(const TreeNode& treeNode, CameraController& camController, std::vector<RAYX::DesignElement>& rObjects,
                         std::vector<glm::dvec3>& rSourcePositions, UIBeamlineInfo& beamlineInfo) const;
    void buildTreeFromXMLNode(rapidxml::xml_node<>* node, TreeNode& treeNode);
    void renderImGuiTreeFromRML(const std::filesystem::path& filename, CameraController& camController, std::vector<RAYX::DesignElement>& rObjects,
                                std::vector<glm::dvec3>& rSourcePositions, UIBeamlineInfo& beamlineInfo);
};