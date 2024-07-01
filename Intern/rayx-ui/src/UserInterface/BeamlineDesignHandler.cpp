#include "BeamlineDesignHandler.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

#include "Beamline/StringConversion.h"
#include "Debug/Instrumentor.h"

void BeamlineDesignHandler::showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    if (uiBeamlineInfo.selectedType == 0) {  // source
        auto sourceParameters = uiBeamlineInfo.sources[uiBeamlineInfo.selectedIndex].m_elementParameters;
        showParameters(sourceParameters, uiBeamlineInfo.elementsChanged, uiBeamlineInfo.selectedType);
    } else if (uiBeamlineInfo.selectedType == 1) {  // element
        auto elementParameters = uiBeamlineInfo.elements[uiBeamlineInfo.selectedIndex].m_elementParameters;
        showParameters(elementParameters, uiBeamlineInfo.elementsChanged, uiBeamlineInfo.selectedType);
    } else if (uiBeamlineInfo.selectedType == 2) {  // group
        // Handle group if needed
    }
}

void BeamlineDesignHandler::showParameters(RAYX::DesignMap& parameters, bool& changed, uint32_t type) {
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

    std::vector<std::string> remainingGroups;
    for (const auto& group : existingGroups) {
        if (std::find(orderedKeys.begin(), orderedKeys.end(), group.first) == orderedKeys.end()) {
            remainingGroups.push_back(group.first);
        }
    }
    std::sort(remainingGroups.begin(), remainingGroups.end(),
              [this](const std::string& a, const std::string& b) { return caseInsensitiveCompare(a, b); });

    std::sort(nonGroupedKeys.begin(), nonGroupedKeys.end(),
              [this](const std::string& a, const std::string& b) { return caseInsensitiveCompare(a, b); });

    // Combine all keys for final display order
    orderedKeys.insert(orderedKeys.end(), remainingGroups.begin(), remainingGroups.end());
    orderedKeys.insert(orderedKeys.end(), nonGroupedKeys.begin(), nonGroupedKeys.end());

    std::unordered_set<std::string> unusedKeys = {"entranceArmLength", "exitArmLength", "grazingIncAngle", "AzimuthalAngle"};

    for (const auto& key : orderedKeys) {
        if (unusedKeys.find(key) != unusedKeys.end()) {
            continue;
        }
        if (existingGroups.find(key) != existingGroups.end()) {
            if (ImGui::CollapsingHeader(key.c_str())) {
                ImGui::Indent();

                std::vector<std::string> sortedGroupKeys = existingGroups[key];
                std::sort(sortedGroupKeys.begin(), sortedGroupKeys.end(), [&](const std::string& a, const std::string& b) {
                    return std::find(customOrder.begin(), customOrder.end(), a) < std::find(customOrder.begin(), customOrder.end(), b);
                });

                for (const auto& groupKey : sortedGroupKeys) {
                    RAYX::DesignMap& element = parameters[groupKey];
                    createInputField(groupKey, element, changed, type, 1);  // Start with nesting level 1 for grouped items
                }

                ImGui::Unindent();
            }
        } else {
            // It's a non-grouped parameter
            RAYX::DesignMap& element = parameters[key];
            createInputField(key, element, changed, type, 0);  // Non-grouped items have nesting level 0
        }
    }
}

void BeamlineDesignHandler::createInputField(const std::string& key, RAYX::DesignMap& element, bool& changed, uint32_t type, int nestingLevel = 0) {
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

    // Calculate widths for consistent layout
    float fullWidth = ImGui::GetContentRegionAvail().x;
    float baseInputWidth = fullWidth * 0.6f;
    float inputWidth = baseInputWidth * float(std::pow(0.9, nestingLevel));  // Reduce by 10% for each nesting level
    float rightAlignPosition = fullWidth - inputWidth;

    // Align label to the left
    ImGui::AlignTextToFramePadding();
    if (element.type() != RAYX::ValueType::Map) {
        ImGui::Text("%s:", key.c_str());
        ImGui::SameLine(rightAlignPosition);
    }

    ImGui::PushItemWidth(inputWidth);

    // Generate a unique ID for each input field
    ImGui::PushID(key.c_str());

    if (key == "type") {
        const char** items;
        int itemsCount;

        if (type == 0) {
            items = sourceItems;
            itemsCount = IM_ARRAYSIZE(this->sourceItems);
        } else {
            items = opticalElementItems;
            itemsCount = IM_ARRAYSIZE(this->opticalElementItems);
        }

        std::string currentStr = element.as_string();

        int currentItem = -1;
        for (int i = 0; i < itemsCount; i++) {
            if (currentStr == items[i]) {
                currentItem = i;
                break;
            }
        }

        if (ImGui::BeginCombo("##combo", currentItem >= 0 ? items[currentItem] : "")) {
            for (int n = 0; n < itemsCount; n++) {
                bool isSelected = (currentItem == n);
                if (ImGui::Selectable(items[n], isSelected)) {
                    element = std::string(items[n]);
                    changed = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

    } else if (key == "geometricalShape") {
        const char* shapes[] = {"Rectangle", "Elliptical", "Trapezoid", "Unlimited"};
        int input = int(element.as_double());

        if (ImGui::BeginCombo("##combo", shapes[input])) {
            for (int i = 0; i < IM_ARRAYSIZE(shapes); i++) {
                bool isSelected = (input == i);
                if (ImGui::Selectable(shapes[i], isSelected)) {
                    element = double(i);
                    changed = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

    } else {
        switch (element.type()) {
            case RAYX::ValueType::Double: {
                double input = element.as_double();

                if (std::isnan(input) || std::isinf(input)) {
                    input = 0.0;  // Default value if input is NaN or inf
                }

                double min_range = -1000000.0;
                double max_range = 1000000.0;

                // speed for dragging
                float speed = 0.1f;

                if (ImGui::DragScalar("##double", ImGuiDataType_Double, &input, speed, &min_range, &max_range, "%.3f", flags)) {
                    if (!std::isnan(input) && !std::isinf(input)) {
                        element = input;
                        changed = true;
                    } else {
                        std::cout << "Invalid input detected, not updating element." << std::endl;
                    }
                }
                break;
            }

            case RAYX::ValueType::Int: {
                int input = element.as_int();
                if (ImGui::InputInt("##int", &input, 0, 0, flags)) {
                    element = input;
                    changed = true;
                }
                break;
            }
            case RAYX::ValueType::Bool: {
                bool input = element.as_bool();
                if (ImGui::Checkbox("##bool", &input)) {
                    element = input;
                    changed = true;
                }
                break;
            }
            case RAYX::ValueType::String: {
                std::string input = element.as_string();
                char buffer[256];
#if defined(WIN32)
                strncpy_s(buffer, input.c_str(), sizeof(buffer));
#else
                strncpy(buffer, input.c_str(), sizeof(buffer));
#endif
                buffer[sizeof(buffer) - 1] = 0;
                if (ImGui::InputText("##string", buffer, sizeof(buffer), flags)) {
                    element = std::string(buffer);
                    changed = true;
                }
                break;
            }
            case RAYX::ValueType::Dvec4: {
                auto currentValue = element.as_dvec4();
                double vals[4] = {currentValue.x, currentValue.y, currentValue.z, currentValue.w};
                bool changedLocal = false;
                for (int i = 0; i < 4; ++i) {
                    ImGui::PushID(i);
                    if (ImGui::InputDouble("##dvec4", &vals[i], 0.0, 0.0, "%.3f", flags)) {
                        changedLocal = true;
                    }
                    ImGui::PopID();
                    if (i < 3) ImGui::SameLine();
                }
                if (changedLocal) {
                    element = dvec4(vals[0], vals[1], vals[2], vals[3]);
                    changed = true;
                }
                break;
            }
            case RAYX::ValueType::Dmat4x4: {
                auto currentValue = element.as_dmat4x4();
                bool changedLocal = false;
                for (int row = 0; row < 4; ++row) {
                    for (int col = 0; col < 4; ++col) {
                        ImGui::PushID(row * 4 + col);
                        double val = currentValue[row][col];
                        if (ImGui::InputDouble("##dmat4x4", &val, 0.0, 0.0, "%.3f", flags)) {
                            currentValue[row][col] = val;
                            changedLocal = true;
                        }
                        ImGui::PopID();
                        if (col < 3) ImGui::SameLine();
                    }
                    if (row < 3) ImGui::NewLine();
                }
                if (changedLocal) {
                    element = currentValue;
                    changed = true;
                }
                break;
            }
            case RAYX::ValueType::SpreadType: {
                auto currentValue = element.as_energySpreadType();
                if (ImGui::BeginCombo("##spreadtype", SpreadTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : SpreadTypeToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::EnergyDistributionType: {
                auto currentValue = element.as_energyDistType();
                if (ImGui::BeginCombo("##energydisttype", EnergyDistributionTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : EnergyDistributionTypeToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::SourceDist: {
                auto currentValue = element.as_sourceDist();
                if (ImGui::BeginCombo("##sourcedist", SourceDistToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : SourceDistToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::ElectronEnergyOrientation: {
                auto currentValue = element.as_electronEnergyOrientation();
                if (ImGui::BeginCombo("##electronenergyorientation", ElectronEnergyOrientationToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : ElectronEnergyOrientationToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::EnergySpreadUnit: {
                auto currentValue = element.as_energySpreadUnit();
                if (ImGui::BeginCombo("##energyspreadunit", EnergySpreadUnitToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : EnergySpreadUnitToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::SigmaType: {
                auto currentValue = element.as_sigmaType();
                if (ImGui::BeginCombo("##sigmatype", SigmaTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : SigmaTypeToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::Rad: {
                Rad currentValue = element.as_rad();
                double input = currentValue.rad;
                if (ImGui::InputDouble("##rad", &input, 0.0, 0.0, "%.6f", flags)) {
                    element = Rad(input);
                    changed = true;
                }
                break;
            }
            case RAYX::ValueType::Material: {
                Material currentValue = element.as_material();
                if (ImGui::BeginCombo("##material", MaterialToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : MaterialToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::CentralBeamstop: {
                auto currentValue = element.as_centralBeamStop();
                if (ImGui::BeginCombo("##centralbeamstop", CentralBeamstopToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : CentralBeamstopToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::FigureRotation: {
                auto currentValue = element.as_figureRotation();
                if (ImGui::BeginCombo("##figurerotation", FigureRotationToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : FigureRotationToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::CurvatureType: {
                auto currentValue = element.as_curvatureType();
                if (ImGui::BeginCombo("##curvaturetype", CurvatureTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : CurvatureTypeToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::BehaviourType: {
                auto currentValue = element.as_behaviourType();
                if (ImGui::BeginCombo("##behaviourtype", BehaviourTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : BehaviourTypeToString) {
                        bool isSelected = (currentValue == value);
                        if (ImGui::Selectable(name.c_str(), isSelected)) {
                            element = value;
                            changed = true;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case RAYX::ValueType::Map: {
                auto currentValue = element.as_map();
                if (ImGui::CollapsingHeader(key.c_str())) {
                    ImGui::Indent();

                    // Create a vector of keys and sort them alphabetically (case-insensitive)
                    std::vector<std::string> keys;
                    keys.reserve(currentValue.size());
                    for (const auto& [subKey, valuePtr] : currentValue) {
                        keys.push_back(subKey);
                    }
                    std::sort(keys.begin(), keys.end(), [this](const std::string& a, const std::string& b) { return caseInsensitiveCompare(a, b); });

                    // Iterate through the sorted keys
                    for (const auto& subKey : keys) {
                        ImGui::PushID(subKey.c_str());
                        bool subChanged = false;
                        createInputField(subKey, *currentValue[subKey], subChanged, type, nestingLevel + 1);
                        if (subChanged) {
                            changed = true;
                            currentValue[subKey] = currentValue[subKey];
                        }
                        ImGui::PopID();
                    }

                    ImGui::Unindent();
                    if (changed) element = currentValue;
                }
                break;
            }

            case RAYX::ValueType::Misalignment:
            case RAYX::ValueType::Surface:
            case RAYX::ValueType::Cutout:
            case RAYX::ValueType::CylinderDirection:
                break;
            default:
                // ImGui::SameLine(rightAlignPosition);
                ImGui::Text("Unsupported type");
                break;
        }
    }
    ImGui::PopID();
    ImGui::PopItemWidth();
}

// Sort remaining groups and non-grouped keys alphabetically (case-insensitive)
bool BeamlineDesignHandler::caseInsensitiveCompare(const std::string& a, const std::string& b) {
    std::string lowerA = a;
    std::string lowerB = b;
    std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
    std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
    return lowerA < lowerB;
};