#include "BeamlineOutliner.h"

#include <imgui.h>

#include <fstream>

BeamlineOutliner::BeamlineOutliner(/* args */) {}

BeamlineOutliner::~BeamlineOutliner() {}

void BeamlineOutliner::renderImGuiTree(const TreeNode& treeNode, CameraController& camController, std::vector<RAYX::OpticalElement>& elements,
                                       std::vector<glm::dvec3>& rSourcePositions) const {
    for (auto& child : treeNode.children) {
        if (child.children.empty()) {
            std::string label = child.name;

            if (ImGui::Selectable(label.c_str())) {
                // Handle selection logic here
                RAYX_VERB << "Selected object: " << child.name << " with index " << child.index;
                if (child.category == "Optical Element") {
                    glm::vec3 translationVec = {elements[child.index].m_element.m_outTrans[3][0], elements[child.index].m_element.m_outTrans[3][1],
                                                elements[child.index].m_element.m_outTrans[3][2]};
                    camController.lookAtPoint(translationVec);
                } else if (child.category == "Light Source") {
                    camController.lookAtPoint(rSourcePositions[child.index]);
                }
            }
        } else {
            if (ImGui::TreeNode(child.name.c_str())) {
                this->renderImGuiTree(child, camController, elements, rSourcePositions);
                ImGui::TreePop();
            }
        }
    }
}

void BeamlineOutliner::buildTreeFromXMLNode(rapidxml::xml_node<>* node, TreeNode& treeNode) {
    for (rapidxml::xml_node<>* xmlChild = node->first_node(); xmlChild; xmlChild = xmlChild->next_sibling()) {
        rapidxml::xml_attribute<>* typeAttr = xmlChild->first_attribute("type");
        std::string type = typeAttr ? typeAttr->value() : "";
        std::string category;

        if (strcmp(xmlChild->name(), "object") == 0) {
            if (type == "Point Source" || type == "Matrix Source" || type == "Dipole" || type == "Dipole Source" || type == "Pixel Source" ||
                type == "Circle Source") {
                category = "Light Source";
                TreeNode objectNode(xmlChild->first_attribute("name")->value(), type, category);
                objectNode.index = m_lightSourceIndex++;
                treeNode.children.emplace_back(objectNode);
            } else if (type == "ImagePlane" || type == "Plane Mirror" || type == "Toroid" || type == "Slit" || type == "Spherical Grating" ||
                       type == "Plane Grating" || type == "Sphere" || type == "Reflection Zoneplate" || type == "Ellipsoid" || type == "Cylinder" ||
                       type == "Cone") {
                category = "Optical Element";
                TreeNode objectNode(xmlChild->first_attribute("name")->value(), type, category);
                objectNode.index = m_opticalElementIndex++;
                treeNode.children.emplace_back(objectNode);
            } else {
                TreeNode objectNode(xmlChild->first_attribute("name")->value(), type, category);
                treeNode.children.emplace_back(objectNode);
            }
        } else if (strcmp(xmlChild->name(), "group") == 0) {
            category = "Group";
            TreeNode groupNode(xmlChild->first_attribute("name")->value(), "", category);
            buildTreeFromXMLNode(xmlChild, groupNode);
            treeNode.children.push_back(groupNode);
        }
    }
}

void BeamlineOutliner::renderImGuiTreeFromRML(const std::filesystem::path& filename, CameraController& camController,
                                              std::vector<RAYX::OpticalElement>& elements, std::vector<glm::dvec3>& rSourcePositions) {
    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        ImGui::Text("Choose a file to display the beamline outline.");
        return;
    }

    // Read and parse the RML file
    std::ifstream fileContent(filename);
    if (!fileContent.is_open()) {
        ImGui::Text("Error: Could not open file.");
        return;
    }

    std::stringstream buffer;
    buffer << fileContent.rdbuf();
    std::string test = buffer.str();

    // Check if the file is empty
    if (test.empty()) {
        ImGui::Text("Error: File is empty.");
        return;
    }

    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;

    try {
        doc.parse<0>(cstr.data());
    } catch (rapidxml::parse_error& e) {
        ImGui::Text("Error: XML Parsing failed:");
        ImGui::Text("%s", e.what());
        return;
    }

    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");
    if (xml_beamline == nullptr) {
        ImGui::Text("Error: <beamline> not found in XML.");
        return;
    }

    // Call recursive function to handle the object collection and render the ImGui tree
    m_pTreeRoot = std::make_unique<TreeNode>("Root");
    buildTreeFromXMLNode(xml_beamline, *m_pTreeRoot);
    renderImGuiTree(*m_pTreeRoot, camController, elements, rSourcePositions);
}

void BeamlineOutliner::showBeamlineOutlineWindow(UIParameters& uiParams, std::vector<RAYX::OpticalElement>& elements,
                                                 std::vector<glm::dvec3>& rSourcePositions) {
    ImGui::SetNextWindowPos(ImVec2(0, 760), ImGuiCond_Once);  // Position it below the Settings window
    ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_Once);

    ImGui::Begin("Beamline Outline");

    if (uiParams.pathChanged) {
        // Create and render new Tree
        m_lightSourceIndex = 0;
        m_opticalElementIndex = 0;
        renderImGuiTreeFromRML(uiParams.rmlPath, uiParams.camController, elements, rSourcePositions);
    } else if (m_pTreeRoot == nullptr) {
        // Do nothing
        ImGui::Text("Choose a file to display the beamline outline.");
    } else {
        // Render same Tree
        renderImGuiTree(*m_pTreeRoot, uiParams.camController, elements, rSourcePositions);
    }

    ImGui::End();
}
