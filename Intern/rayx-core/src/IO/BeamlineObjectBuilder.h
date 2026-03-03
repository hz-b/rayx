#pragma once

#include <filesystem>

#include "Design/Beamline.h"
#include "Core.h"

namespace rayx {

void makeRotationAndTranslation(rapidxml::xml_node<>* node, Node& parent);

void makeArea(rapidxml::xml_node<>* node, Area& area, const std::filesystem::path& filepath);

void makeBehavior(rapidxml::xml_node<>* node, Behavior& behavior);

void makeOptionalCurvature(rapidxml::xml_node<>* node, SurfaceElement& se);

void makeOptionalAperture(rapidxml::xml_node<>* node, SurfaceElement& se);

void makeOptionalSlopeError(rapidxml::xml_node<>* node, SurfaceElement& se);

void makeDipoleSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);

void makePixelSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);

void makeCircleSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);

void makeSimpleUndulatorSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);

void makePointSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);

void makeMatrixSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);

void makeInputSource(rapidxml::xml_node<>* node, SourceNode& source, const std::filesystem::path& filepath);


}