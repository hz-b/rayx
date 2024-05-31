#include "BeamlineDesignHandler.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

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
                // Begin a child window with a border
                ImGui::BeginChild((key + "_child").c_str(), ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

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
        // Handle other custom types if needed
        case RAYX::ValueType::Rad:
            // Add custom input handling for Rad type
            break;
        case RAYX::ValueType::Material:
            // Add custom input handling for Material type
            break;
        case RAYX::ValueType::Misalignment:
            // Add custom input handling for Misalignment type
            break;
        case RAYX::ValueType::CentralBeamstop:
            // Add custom input handling for CentralBeamstop type
            break;
        case RAYX::ValueType::Cutout:
            // Add custom input handling for Cutout type
            break;
        case RAYX::ValueType::CylinderDirection:
            // Add custom input handling for CylinderDirection type
            break;
        case RAYX::ValueType::FigureRotation:
            // Add custom input handling for FigureRotation type
            break;
        case RAYX::ValueType::CurvatureType:
            // Add custom input handling for CurvatureType type
            break;
        case RAYX::ValueType::Surface:
            // Add custom input handling for Surface type
            break;
        case RAYX::ValueType::BehaviourType:
            // Add custom input handling for BehaviourType type
            break;
        default:
            break;
    }

    ImGui::PopItemWidth();  // Restore the previous item width
}
