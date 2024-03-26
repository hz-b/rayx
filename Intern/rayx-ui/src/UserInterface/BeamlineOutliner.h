#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Beamline/OpticalElement.h"
#include "UserInterface/Settings.h"

// Simple TreeNode
struct TreeNode {
    std::string name;
    std::string type;
    std::string category;  // Add this field for category
    int index = -1;
    std::vector<TreeNode> children;

    TreeNode(const std::string& name, const std::string& type = "", const std::string& category = "") : name(name), type(type), category(category) {}
};

class BeamlineOutliner {
  public:
    BeamlineOutliner();
    ~BeamlineOutliner();

    void showBeamlineOutlineWindow(UIParameters& uiParams, std::vector<RAYX::DesignElement>& elements, std::vector<glm::dvec3>& rSourcePositions);

  private:
    std::unique_ptr<TreeNode> m_pTreeRoot = nullptr;

    int m_lightSourceIndex = 0;
    int m_opticalElementIndex = 0;

    void renderImGuiTree(const TreeNode& treeNode, CameraController& camController, std::vector<RAYX::DesignElement>& rObjects,
                         std::vector<glm::dvec3>& rSourcePositions) const;
    void buildTreeFromXMLNode(rapidxml::xml_node<>* node, TreeNode& treeNode);
    void renderImGuiTreeFromRML(const std::filesystem::path& filename, CameraController& camController, std::vector<RAYX::DesignElement>& rObjects,
                                std::vector<glm::dvec3>& rSourcePositions);
};