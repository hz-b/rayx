#include "BeamlineDesignHandler.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

void BeamlineDesignHandler::showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo) {
    if (uiBeamlineInfo.selectedType == 0) {  // source
        auto sourceParameters = uiBeamlineInfo.sources[uiBeamlineInfo.selectedIndex].m_elementParameters;
        for (auto& elementPair : sourceParameters) {
            const std::string& key = elementPair.first;
            RAYX::DesignMap& element = *(elementPair.second);

            createInputField(key, element, uiBeamlineInfo.elementsChanged);
        }
    } else if (uiBeamlineInfo.selectedType == 1) {  // element
        auto elementParameters = uiBeamlineInfo.elements[uiBeamlineInfo.selectedIndex].m_elementParameters;
        for (auto& elementPair : elementParameters) {
            const std::string& key = elementPair.first;
            RAYX::DesignMap& element = *(elementPair.second);

            createInputField(key, element, uiBeamlineInfo.elementsChanged);
        }
    } else if (uiBeamlineInfo.selectedType == 2) {  // group
    }
}

void BeamlineDesignHandler::createInputField(const std::string& key, DesignMap& element, bool& changed) {
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

    switch (element.type()) {
        case RAYX::ValueType::Double: {
            double input = element.as_double();
            if (ImGui::InputDouble(key.c_str(), &input, 0.0, 0.0, "%.6f", flags)) {
                element = input;
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::Int: {
            int input = element.as_int();
            if (ImGui::InputInt(key.c_str(), &input, 0, 0, flags)) {
                element = input;
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::Bool: {
            bool input = element.as_bool();
            if (ImGui::Checkbox(key.c_str(), &input)) {
                element = input;
                changed = true;
            }
            break;
        }
        case RAYX::ValueType::String: {
            std::string input = element.as_string();
            char buffer[256];
            strncpy(buffer, input.c_str(), sizeof(buffer));
            if (ImGui::InputText(key.c_str(), buffer, sizeof(buffer), flags)) {
                element = std::string(buffer);
                changed = true;
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
        case RAYX::ValueType::SourceDist:
            // Add custom input handling for SourceDist type
            break;
        case RAYX::ValueType::SpreadType:
            // Add custom input handling for SpreadType type
            break;
        case RAYX::ValueType::EnergyDistributionType:
            // Add custom input handling for EnergyDistributionType type
            break;
        case RAYX::ValueType::EnergySpreadUnit:
            // Add custom input handling for EnergySpreadUnit type
            break;
        case RAYX::ValueType::ElectronEnergyOrientation:
            // Add custom input handling for ElectronEnergyOrientation type
            break;
        case RAYX::ValueType::SigmaType:
            // Add custom input handling for SigmaType type
            break;
        case RAYX::ValueType::BehaviourType:
            // Add custom input handling for BehaviourType type
            break;
        default:
            break;
    }
}
