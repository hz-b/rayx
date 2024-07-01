#include "BeamlineOutliner.h"

#include <imgui.h>

#include <fstream>

BeamlineOutliner::BeamlineOutliner(/* args */) {}

BeamlineOutliner::~BeamlineOutliner() {}

void BeamlineOutliner::renderImGuiTree(const TreeNode& treeNode, CameraController& camController, std::vector<RAYX::DesignElement>& elements,
                                       std::vector<glm::dvec3>& rSourcePositions, UIBeamlineInfo& beamlineInfo) const {
    for (auto& child : treeNode.children) {
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (child.children.empty()) {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool isSelected = (beamlineInfo.selectedIndex == child.index) && ((beamlineInfo.selectedType == 0 && child.category == "Light Source") ||
                                                                          (beamlineInfo.selectedType == 1 && child.category == "Optical Element") ||
                                                                          (beamlineInfo.selectedType == 2 && !child.children.empty()));

        if (isSelected) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4((84.0f / 256.0f), (84.0f / 256.0f), (84.0f / 256.0f), 1.00f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4((84.0f / 256.0f), (84.0f / 256.0f), (84.0f / 256.0f), 1.00f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4((84.0f / 256.0f), (84.0f / 256.0f), (84.0f / 256.0f), 1.00f));
        }

        // Create a unique ID for the button
        std::string buttonId = "<--##" + child.name + std::to_string(child.index);

        if (ImGui::Button(buttonId.c_str())) {
            if (child.category == "Optical Element" && child.index >= 0 && static_cast<size_t>(child.index) < elements.size()) {
                glm::vec3 translationVec = {elements[child.index].compile().m_outTrans[3][0], elements[child.index].compile().m_outTrans[3][1],
                                            elements[child.index].compile().m_outTrans[3][2]};
                camController.lookAtPoint(translationVec);
            } else if (child.category == "Light Source" && child.index >= 0 && static_cast<size_t>(child.index) < rSourcePositions.size()) {
                camController.lookAtPoint(rSourcePositions[child.index]);
            }
        }

        ImGui::SameLine();

        std::string nodeLabel = child.name + "##" + std::to_string(child.index);
        bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), nodeFlags);

        if (isSelected) {
            ImGui::PopStyleColor(3);
        }

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            RAYX_VERB << "Selected object: " << child.name << " with index " << child.index;

            if (child.category == "Light Source") {
                beamlineInfo.selectedType = 0;
            } else if (child.category == "Optical Element") {
                beamlineInfo.selectedType = 1;
            } else {
                beamlineInfo.selectedType = 2;
            }
            beamlineInfo.selectedIndex = child.index;
        }

        if (nodeOpen && !child.children.empty()) {
            this->renderImGuiTree(child, camController, elements, rSourcePositions, beamlineInfo);
            ImGui::TreePop();
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
                                              std::vector<RAYX::DesignElement>& elements, std::vector<glm::dvec3>& rSourcePositions,
                                              UIBeamlineInfo& beamlineInfo) {
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
    renderImGuiTree(*m_pTreeRoot, camController, elements, rSourcePositions, beamlineInfo);
}

void BeamlineOutliner::showBeamlineOutlineWindow(UIParameters& uiParams) {
    ImGui::Begin("Beamline Outline");

    if (uiParams.rmlPath.empty()) {
        ImGui::Text("Choose a file to display the beamline outline.");
    } else if (uiParams.rmlPath != m_currentRML) {
        // Create and render new Tree
        m_lightSourceIndex = 0;
        m_opticalElementIndex = 0;
        renderImGuiTreeFromRML(uiParams.rmlPath, uiParams.camController, uiParams.beamlineInfo.elements, uiParams.beamlineInfo.rSourcePositions,
                               uiParams.beamlineInfo);
        m_currentRML = uiParams.rmlPath;
    } else if (m_pTreeRoot == nullptr) {
        RAYX_ERR << "Error: Tree root is null.";
    } else {
        // Render same Tree
        renderImGuiTree(*m_pTreeRoot, uiParams.camController, uiParams.beamlineInfo.elements, uiParams.beamlineInfo.rSourcePositions,
                        uiParams.beamlineInfo);
    }

    ImGui::End();
}
