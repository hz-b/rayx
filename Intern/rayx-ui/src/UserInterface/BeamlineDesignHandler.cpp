#include "BeamlineDesignHandler.h"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

#include "Beamline/Beamline.h"
#include "Beamline/StringConversion.h"
#include "Debug/Instrumentor.h"
#include "Element/Element.h"

void BeamlineDesignHandler::showBeamlineDesignWindow(UIBeamlineInfo& uiInfo) {
    if (!uiInfo.selectedNode) {
        ImGui::Text("No node selected");
        return;
    }

    if (uiInfo.selectedNode->isSource()) {
        const auto srcPtr = static_cast<rayx::DesignSource*>(uiInfo.selectedNode);
        if (srcPtr) { showParameters(srcPtr->m_elementParameters, uiInfo.elementsChanged, SelectedType::LightSource); }
    } else if (uiInfo.selectedNode->isElement()) {
        const auto elemPtr = static_cast<rayx::DesignElement*>(uiInfo.selectedNode);
        if (elemPtr) { showParameters(elemPtr->m_elementParameters, uiInfo.elementsChanged, SelectedType::OpticalElement); }
    } else if (uiInfo.selectedNode->isGroup()) {
        ImGui::Text("Group editing is to be implemented still...");
    } else {
        throw std::runtime_error("Tree element of unknown type encountered!");
    }
}

void BeamlineDesignHandler::showParameters(rayx::DesignMap& parameters, bool& changed, SelectedType type) {
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

        if (!isGrouped) { nonGroupedKeys.push_back(key); }
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
    std::vector<std::string> remainingGroups;
    for (const auto& group : existingGroups) {
        if (std::find(orderedKeys.begin(), orderedKeys.end(), group.first) == orderedKeys.end()) { remainingGroups.push_back(group.first); }
    }
    std::sort(remainingGroups.begin(), remainingGroups.end(),
              [this](const std::string& a, const std::string& b) { return caseInsensitiveCompare(a, b); });

    std::sort(nonGroupedKeys.begin(), nonGroupedKeys.end(),
              [this](const std::string& a, const std::string& b) { return caseInsensitiveCompare(a, b); });

    // Combine all keys for final display order
    orderedKeys.insert(orderedKeys.end(), remainingGroups.begin(), remainingGroups.end());
    orderedKeys.insert(orderedKeys.end(), nonGroupedKeys.begin(), nonGroupedKeys.end());

    // these are not used in RAYX
    std::unordered_set<std::string> unusedKeys = {"photonFlux", "distancePreceding"};

    for (const auto& key : orderedKeys) {
        if (unusedKeys.find(key) != unusedKeys.end()) { continue; }
        if (existingGroups.find(key) != existingGroups.end()) {
            if (ImGui::CollapsingHeader(key.c_str())) {
                ImGui::Indent();

                std::vector<std::string> sortedGroupKeys = existingGroups[key];
                std::sort(sortedGroupKeys.begin(), sortedGroupKeys.end(), [&](const std::string& a, const std::string& b) {
                    return std::find(customOrder.begin(), customOrder.end(), a) < std::find(customOrder.begin(), customOrder.end(), b);
                });

                for (const auto& groupKey : sortedGroupKeys) {
                    rayx::DesignMap& element = parameters[groupKey];
                    createInputField(groupKey, element, changed, type,
                                     1);  // Start with nesting level 1 for grouped items
                }

                ImGui::Unindent();
            }
        } else {
            // It's a non-grouped parameter
            rayx::DesignMap& element = parameters[key];
            createInputField(key, element, changed, type, 0);  // Non-grouped items have nesting level 0
        }
    }
}

void BeamlineDesignHandler::createInputField(const std::string& key, rayx::DesignMap& element, bool& changed, SelectedType type,
                                             int nestingLevel = 0) {
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

    // Calculate widths for consistent layout
    float fullWidth          = ImGui::GetContentRegionAvail().x;
    float baseInputWidth     = fullWidth * 0.6f;
    float inputWidth         = baseInputWidth * float(std::pow(0.9, nestingLevel));  // Reduce by 10% for each nesting level
    float rightAlignPosition = fullWidth - inputWidth;

    // Align label to the left
    ImGui::AlignTextToFramePadding();
    if (element.type() != rayx::ValueType::Map) {
        ImGui::Text("%s:", key.c_str());
        // Align input field to the right
        ImGui::SameLine(rightAlignPosition);
    }

    ImGui::PushItemWidth(inputWidth);

    // Generate a unique ID for each input field
    ImGui::PushID(key.c_str());

    // type,geometricalShape and openingShape need to be a drowdown instead of a string/int input
    if (key == "type") {
        auto currentEl  = element.as_elementType();
        int currentItem = int(std::distance(rayx::ElementTypeToString.begin(), rayx::ElementTypeToString.find(currentEl)));

        static bool isDisabled = true;  // TODO: Enable after SRI release has been built
        if (true) { ImGui::BeginDisabled(); }

        if (ImGui::BeginCombo("##combo", currentItem >= 0 ? rayx::ElementTypeToString.at(currentEl).c_str() : "")) {
            [[maybe_unused]] int n = 0;
            for (const auto& pair : rayx::ElementTypeToString) {
                bool isSelected = (currentEl == pair.first);
                if (ImGui::Selectable(pair.second.c_str(), isSelected)) {
                    element = pair.first;
                    changed = true;
                }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
                n++;
            }
            ImGui::EndCombo();
        }

        if (isDisabled) { ImGui::EndDisabled(); }
    } else if (key == "geometricalShape" || key == "openingShape") {
        const char* shapesGeometrical[] = {"Rectangle", "Elliptical", "Trapezoid", "Unlimited"};
        const char* shapesOpening[]     = {"Rectangle", "Elliptical", "Unlimited"};

        const char** shapes;
        int numShapes;

        if (key == "geometricalShape") {
            shapes    = shapesGeometrical;
            numShapes = IM_ARRAYSIZE(shapesGeometrical);
        } else {  // key == "openingShape"
            shapes    = shapesOpening;
            numShapes = IM_ARRAYSIZE(shapesOpening);
        }

        int input = int(element.as_double());

        if (ImGui::BeginCombo("##combo", shapes[input])) {
            for (int i = 0; i < numShapes; i++) {
                bool isSelected = (input == i);
                if (ImGui::Selectable(shapes[i], isSelected)) {
                    element = double(i);
                    changed = true;
                }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }
            ImGui::EndCombo();
        }
    } else {
        // dynamic handling of different types
        switch (element.type()) {
            case rayx::ValueType::Double: {
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

            case rayx::ValueType::Int: {
                int input = element.as_int();
                if (ImGui::InputInt("##int", &input, 0, 0, flags)) {
                    element = input;
                    changed = true;
                }
                break;
            }
            case rayx::ValueType::Bool: {
                bool input = element.as_bool();
                if (ImGui::Checkbox("##bool", &input)) {
                    element = input;
                    changed = true;
                }
                break;
            }
            case rayx::ValueType::String: {
                static bool isDisabled = true;  // TODO: Enable after SRI release has been built
                if (true) { ImGui::BeginDisabled(); }

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
                if (isDisabled) { ImGui::EndDisabled(); }
                break;
            }
            case rayx::ValueType::Dvec4: {
                auto currentValue = element.as_dvec4();
                double vals[4]    = {currentValue.x, currentValue.y, currentValue.z, currentValue.w};
                bool changedLocal = false;
                for (int i = 0; i < 4; ++i) {
                    ImGui::PushID(i);
                    if (ImGui::InputDouble("##dvec4", &vals[i], 0.0, 0.0, "%.3f", flags)) { changedLocal = true; }
                    ImGui::PopID();
                    if (i < 3) ImGui::SameLine();
                }
                if (changedLocal) {
                    element = glm::dvec4(vals[0], vals[1], vals[2], vals[3]);
                    changed = true;
                }
                break;
            }
            case rayx::ValueType::Dmat4x4: {
                auto currentValue = element.as_dmat4x4();
                bool changedLocal = false;
                for (int row = 0; row < 4; ++row) {
                    for (int col = 0; col < 4; ++col) {
                        ImGui::PushID(row * 4 + col);
                        double val = currentValue[row][col];
                        if (ImGui::InputDouble("##dmat4x4", &val, 0.0, 0.0, "%.3f", flags)) {
                            currentValue[row][col] = val;
                            changedLocal           = true;
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
            case rayx::ValueType::SpreadType: {
                auto currentValue = element.as_energySpreadType();
                if (ImGui::BeginCombo("##spreadtype", rayx::SpreadTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::SpreadTypeToString) {
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
            case rayx::ValueType::EnergyDistributionType: {
                auto currentValue = element.as_energyDistributionType();
                if (ImGui::BeginCombo("##energydisttype", rayx::EnergyDistributionTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::EnergyDistributionTypeToString) {
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
            case rayx::ValueType::SourceDist: {
                auto currentValue = element.as_sourceDist();
                if (ImGui::BeginCombo("##sourcedist", rayx::SourceDistToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::SourceDistToString) {
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
            case rayx::ValueType::ElectronEnergyOrientation: {
                auto currentValue = element.as_electronEnergyOrientation();
                if (ImGui::BeginCombo("##electronenergyorientation", rayx::ElectronEnergyOrientationToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::ElectronEnergyOrientationToString) {
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
            case rayx::ValueType::EnergySpreadUnit: {
                auto currentValue = element.as_energySpreadUnit();
                if (ImGui::BeginCombo("##energyspreadunit", rayx::EnergySpreadUnitToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::EnergySpreadUnitToString) {
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
            case rayx::ValueType::SigmaType: {
                auto currentValue = element.as_sigmaType();
                if (ImGui::BeginCombo("##sigmatype", rayx::SigmaTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::SigmaTypeToString) {
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
            case rayx::ValueType::Rad: {
                rayx::Rad currentValue = element.as_rad();
                double input           = currentValue.rad;
                if (ImGui::InputDouble("##rad", &input, 0.0, 0.0, "%.6f", flags)) {
                    element = rayx::Rad(input);
                    changed = true;
                }
                break;
            }
            case rayx::ValueType::Material: {
                rayx::Material currentValue = element.as_material();
                if (ImGui::BeginCombo("##material", rayx::MaterialToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::MaterialToString) {
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
            case rayx::ValueType::CentralBeamstop: {
                auto currentValue = element.as_centralBeamStop();
                if (ImGui::BeginCombo("##centralbeamstop", rayx::CentralBeamstopToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::CentralBeamstopToString) {
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
            case rayx::ValueType::FigureRotation: {
                auto currentValue = element.as_figureRotation();
                if (ImGui::BeginCombo("##figurerotation", rayx::FigureRotationToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::FigureRotationToString) {
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
            case rayx::ValueType::CurvatureType: {
                auto currentValue = element.as_curvatureType();
                if (ImGui::BeginCombo("##curvaturetype", rayx::CurvatureTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::CurvatureTypeToString) {
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
            case rayx::ValueType::BehaviourType: {
                auto currentValue = element.as_behaviourType();
                if (ImGui::BeginCombo("##behaviourtype", rayx::BehaviourTypeToString.at(currentValue).c_str())) {
                    for (const auto& [value, name] : rayx::BehaviourTypeToString) {
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
            // manual sorting is not yet implemented for the map type
            case rayx::ValueType::Map: {
                auto currentValue = element.as_map();
                if (ImGui::CollapsingHeader(key.c_str())) {
                    ImGui::Indent();

                    // Create a vector of keys and sort them alphabetically (case-insensitive)
                    std::vector<std::string> keys;
                    keys.reserve(currentValue.size());
                    for (const auto& [subKey, valuePtr] : currentValue) { keys.push_back(subKey); }
                    std::sort(keys.begin(), keys.end(), [this](const std::string& a, const std::string& b) { return caseInsensitiveCompare(a, b); });

                    // Iterate through the sorted keys
                    for (const auto& subKey : keys) {
                        ImGui::PushID(subKey.c_str());
                        bool subChanged = false;
                        createInputField(subKey, *currentValue[subKey], subChanged, type, nestingLevel + 1);
                        if (subChanged) {
                            changed              = true;
                            currentValue[subKey] = currentValue[subKey];
                        }
                        ImGui::PopID();
                    }

                    ImGui::Unindent();
                    if (changed) element = currentValue;
                }
                break;
            }

            // this needs a lot of parameters and handling
            case rayx::ValueType::Surface:

            // not sure if needed
            case rayx::ValueType::Cutout:
            case rayx::ValueType::CylinderDirection:
                break;
            default:
                ImGui::Text("Unsupported type");
                break;
        }
    }
    ImGui::PopID();
    ImGui::PopItemWidth();
}

bool BeamlineDesignHandler::caseInsensitiveCompare(const std::string& a, const std::string& b) {
    std::string lowerA = a;
    std::string lowerB = b;
    std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
    std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
    return lowerA < lowerB;
};
