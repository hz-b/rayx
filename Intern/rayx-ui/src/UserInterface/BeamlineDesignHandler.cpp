#include "BeamlineDesignHandler.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include "Beamline/StringConversion.h"

void BeamlineDesignHandler::showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo) {
    if (uiBeamlineInfo.selectedType == 0) {  // source
        auto sourceParameters = uiBeamlineInfo.sources[uiBeamlineInfo.selectedIndex].m_elementParameters;
        showParameters(sourceParameters, uiBeamlineInfo.elementsChanged);
    } else if (uiBeamlineInfo.selectedType == 1) {  // element
        auto elementParameters = uiBeamlineInfo.elements[uiBeamlineInfo.selectedIndex].m_elementParameters;
        showParameters(elementParameters, uiBeamlineInfo.elementsChanged);
    } else if (uiBeamlineInfo.selectedType == 2) {  // group
        // Handle group if needed
    }
}

void BeamlineDesignHandler::showParameters(RAYX::DesignMap& parameters, bool& changed) {
    // Collect existing members of each group
    std::map<std::string, std::vector<std::string>> existingGroups;
    std::vector<std::string> nonGroupedKeys;

    for (const auto& elementPair : parameters) {
        const std::string& key = elementPair.first;

        bool isGrouped = false;
        for (const auto& group : groups) {
            if (std::find(group.second.begin(), group.second.end(), key) != group.second.end()) {
                existingGroups[group.first].push_back(key);
                isGrouped = true;
                break;
            }
        }

        if (!isGrouped) {
            nonGroupedKeys.push_back(key);
        }
    }

    // Extract entries specified in the custom order
    std::vector<std::string> orderedKeys;
    for (const auto& key : customOrder) {
        if (existingGroups.find(key) != existingGroups.end() && existingGroups[key].size() >= 2) {
            orderedKeys.push_back(key);  // Add group if it has 2 or more members
        } else if (std::find(nonGroupedKeys.begin(), nonGroupedKeys.end(), key) != nonGroupedKeys.end()) {
            orderedKeys.push_back(key);  // Add non-grouped parameter
            nonGroupedKeys.erase(std::remove(nonGroupedKeys.begin(), nonGroupedKeys.end(), key), nonGroupedKeys.end());
        }
    }

    // Sort remaining groups and non-grouped keys alphabetically (case-insensitive)
    auto caseInsensitiveCompare = [](const std::string& a, const std::string& b) {
        std::string lowerA = a;
        std::string lowerB = b;
        std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
        std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
        return lowerA < lowerB;
    };

    std::vector<std::string> remainingGroups;
    for (const auto& group : existingGroups) {
        if (std::find(orderedKeys.begin(), orderedKeys.end(), group.first) == orderedKeys.end()) {
            remainingGroups.push_back(group.first);
        }
    }
    std::sort(remainingGroups.begin(), remainingGroups.end(), caseInsensitiveCompare);
    std::sort(nonGroupedKeys.begin(), nonGroupedKeys.end(), caseInsensitiveCompare);

    // Combine all keys for final display order
    orderedKeys.insert(orderedKeys.end(), remainingGroups.begin(), remainingGroups.end());
    orderedKeys.insert(orderedKeys.end(), nonGroupedKeys.begin(), nonGroupedKeys.end());

    // Show parameters according to the sorted order
    for (const auto& key : orderedKeys) {
        if (existingGroups.find(key) != existingGroups.end()) {
            // It's a group
            if (ImGui::CollapsingHeader(key.c_str())) {
                // Calculate the height based on the number of elements
                float elementHeight = ImGui::GetTextLineHeightWithSpacing();
                float childHeight = elementHeight * existingGroups[key].size() * 1.6f + 10.0f;
                // Begin a child window with a border and specified height
                ImGui::BeginChild((key + "_child").c_str(), ImVec2(0, childHeight), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

                std::vector<std::string> sortedGroupKeys = existingGroups[key];
                std::sort(sortedGroupKeys.begin(), sortedGroupKeys.end(), [&](const std::string& a, const std::string& b) {
                    return std::find(customOrder.begin(), customOrder.end(), a) < std::find(customOrder.begin(), customOrder.end(), b);
                });

                for (const auto& groupKey : sortedGroupKeys) {
                    RAYX::DesignMap& element = parameters[groupKey];
                    createInputField(groupKey, element, changed);
                }

                ImGui::EndChild();  // End the child window
            }
        } else {
            // It's a non-grouped parameter
            RAYX::DesignMap& element = parameters[key];
            createInputField(key, element, changed);
        }
    }
}

void BeamlineDesignHandler::createInputField(const std::string& key, RAYX::DesignMap& element, bool& changed) {
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

    // Calculate the available width for the input box
    float fullWidth = ImGui::GetContentRegionAvail().x;
    float keyWidth = ImGui::CalcTextSize(key.c_str()).x;
    float availableWidth = fullWidth - keyWidth - 10;  // Adjust the margin as needed

    // Display the key name
    ImGui::TextUnformatted(key.c_str());
    ImGui::SameLine();
    ImGui::PushItemWidth(availableWidth);

    switch (element.type()) {
        case RAYX::ValueType::Double: {
            double input = element.as_double();
            if (ImGui::InputDouble(("##" + key).c_str(), &input, 0.0, 0.0, "%.6f", flags)) {
                element = input;
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::Int: {
            int input = element.as_int();
            if (ImGui::InputInt(("##" + key).c_str(), &input, 0, 0, flags)) {
                element = input;
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::Bool: {
            bool input = element.as_bool();
            if (ImGui::Checkbox(("##" + key).c_str(), &input)) {
                element = input;
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::String: {
            std::string input = element.as_string();
            char buffer[256];
            strncpy(buffer, input.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = 0;  // Ensure null-termination
            if (ImGui::InputText(("##" + key).c_str(), buffer, sizeof(buffer), flags)) {
                element = std::string(buffer);
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::SpreadType: {
            auto currentValue = element.as_energySpreadType();
            std::string currentStr = SpreadTypeToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : SpreadTypeToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::EnergyDistributionType: {
            auto currentValue = element.as_energyDistType();
            std::string currentStr = EnergyDistributionTypeToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : EnergyDistributionTypeToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::SourceDist: {
            auto currentValue = element.as_sourceDist();
            std::string currentStr = SourceDistToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : SourceDistToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::ElectronEnergyOrientation: {
            auto currentValue = element.as_electronEnergyOrientation();
            std::string currentStr = ElectronEnergyOrientationToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : ElectronEnergyOrientationToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::EnergySpreadUnit: {
            auto currentValue = element.as_energySpreadUnit();
            std::string currentStr = EnergySpreadUnitToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : EnergySpreadUnitToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::SigmaType: {
            auto currentValue = element.as_sigmaType();
            std::string currentStr = SigmaTypeToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : SigmaTypeToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::Rad: {
            Rad currentValue = element.as_rad();
            double input = currentValue.rad;
            if (ImGui::InputDouble(("##" + key).c_str(), &input, 0.0, 0.0, "%.6f", flags)) {
                element = Rad(input);
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::Material: {
            Material currentValue = element.as_material();
            std::string currentStr = MaterialToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : MaterialToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }

        case RAYX::ValueType::Misalignment:
            // Ignoring Misalignment as per instructions
            break;
        case RAYX::ValueType::CentralBeamstop: {
            auto currentValue = element.as_centralBeamStop();
            std::string currentStr = CentralBeamstopToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : CentralBeamstopToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::Cutout:
            // TODO
            break;
        case RAYX::ValueType::CylinderDirection:
            // TODO
            break;
        case RAYX::ValueType::FigureRotation: {
            auto currentValue = element.as_figureRotation();
            std::string currentStr = FigureRotationToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : FigureRotationToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::CurvatureType: {
            auto currentValue = element.as_curvatureType();
            std::string currentStr = CurvatureTypeToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : CurvatureTypeToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::Surface:
            // Ignoring Surface as per instructions
            break;
        case RAYX::ValueType::BehaviourType: {
            auto currentValue = element.as_behaviourType();
            std::string currentStr = BehaviourTypeToString.at(currentValue);
            if (ImGui::BeginCombo(("##" + key).c_str(), currentStr.c_str())) {
                for (const auto& [value, name] : BehaviourTypeToString) {
                    bool isSelected = (currentValue == value);
                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        element = value;
                        changed = true;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case RAYX::ValueType::Map: {
            auto currentValue = element.as_map();
            if (ImGui::CollapsingHeader("##", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
                for (const auto& [subKey, valuePtr] : currentValue) {
                    ImGui::PushID(subKey.c_str());      // Use PushID to avoid ID conflicts
                    ImGui::Text("%s", subKey.c_str());  // Display the sub-key as a label
                    ImGui::SameLine();

                    // Dereference the shared pointer to access the actual element
                    auto& value = *valuePtr;

                    if (value.type() == RAYX::ValueType::Double) {
                        double input = value.as_double();  // Access the double value
                        if (ImGui::InputDouble(("##" + subKey).c_str(), &input, 0.0, 0.0, "%.6f", flags)) {
                            value = input;  // Update the value inside the shared pointer
                            changed = true;
                        }
                    }
                    ImGui::PopID();  // Pop the ID to maintain the stack integrity
                }
                element = currentValue;  // Update the element with the modified map
            }
            break;
        }

        case RAYX::ValueType::Dvec4: {
            auto currentValue = element.as_dvec4();
            double vals[4] = {currentValue.x, currentValue.y, currentValue.z, currentValue.w};
            if (ImGui::InputDouble(("##" + key + "_x").c_str(), &vals[0], 0.0, 0.0, "%.6f", flags) ||
                ImGui::InputDouble(("##" + key + "_y").c_str(), &vals[1], 0.0, 0.0, "%.6f", flags) ||
                ImGui::InputDouble(("##" + key + "_z").c_str(), &vals[2], 0.0, 0.0, "%.6f", flags) ||
                ImGui::InputDouble(("##" + key + "_w").c_str(), &vals[3], 0.0, 0.0, "%.6f", flags)) {
                element = dvec4(vals[0], vals[1], vals[2], vals[3]);
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::Dmat4x4: {
            auto currentValue = element.as_dmat4x4();
            double vals[16] = {currentValue[0][0], currentValue[0][1], currentValue[0][2], currentValue[0][3], currentValue[1][0], currentValue[1][1],
                               currentValue[1][2], currentValue[1][3], currentValue[2][0], currentValue[2][1], currentValue[2][2], currentValue[2][3],
                               currentValue[3][0], currentValue[3][1], currentValue[3][2], currentValue[3][3]};
            bool changedLocal = false;
            for (int i = 0; i < 4; ++i) {
                if (ImGui::InputDouble(("##" + key + "_row" + std::to_string(i) + "_col0").c_str(), &vals[i * 4 + 0], 0.0, 0.0, "%.6f", flags) ||
                    ImGui::InputDouble(("##" + key + "_row" + std::to_string(i) + "_col1").c_str(), &vals[i * 4 + 1], 0.0, 0.0, "%.6f", flags) ||
                    ImGui::InputDouble(("##" + key + "_row" + std::to_string(i) + "_col2").c_str(), &vals[i * 4 + 2], 0.0, 0.0, "%.6f", flags) ||
                    ImGui::InputDouble(("##" + key + "_row" + std::to_string(i) + "_col3").c_str(), &vals[i * 4 + 3], 0.0, 0.0, "%.6f", flags)) {
                    changedLocal = true;
                }
            }
            if (changedLocal) {
                element = glm::dmat4x4(vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7], vals[8], vals[9], vals[10], vals[11],
                                       vals[12], vals[13], vals[14], vals[15]);
                changed = true;
            }
            break;
        }
        default:
            // Ignoring unknown types
            // RAYX_LOG << "Ignoring unknown type index: " << element.type();
            break;
    }

    ImGui::PopItemWidth();  // Restore the previous item width
}
