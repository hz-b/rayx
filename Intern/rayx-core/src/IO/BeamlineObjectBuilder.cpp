#pragma once

#include <filesystem>
#include "BeamlineObjectBuilder.h"
#include "Design/Beamline.h"
#include "xml.h"

namespace rayx {

void makeRotationAndTranslation(rayx::xml::Parser parser, Node& parent) {

    glm::dvec3 position = parser.parsePosition();
    glm::dmat3x3 orientation = parser.parseOrientation();

    parent.append(Rotation(orientation));
    parent.append(Translation(position));
}

AperatureArea makeAperatureArea(rayx::xml::Parser parser) {
    AperatureArea area;
    std::string name = parser.name();
    if (name == "Slit") {
        area = RectangularArea{.width = parser.parseWidth(), .height = parser.parseHeight()};
    } else if (name == "Aperture") {
        area = EllipticalArea{.horizontalDiameter = parser.parseHorizontalDiameter(), .verticalDiameter = parser.parseVerticalDiameter()};
    } else 
        RAYX_EXIT << "unknown area type \"" << name << "\"";
    return area;
}

Area makeArea(rayx::xml::Parser parser) {
    Area area;
    std::string name = parser.name();
    if (name == "ImagePlane") {
        return UnlimitedArea{};
    } else if (name == "Slit" || name == "Aperture") {
        return makeAperatureArea(parser);
    } else 
        RAYX_EXIT << "unknown area type \"" << name << "\"";
    }

    rayx::xml::paramEnergyDistribution(parser.node, "energyDistribution", parser.filepath, &area.energyDistribution);
    rayx::xml::paramVls(parser.node, "vlsCoefficients", &area.vlsCoefficients);
    return area;
}

Behavior makeBehavior(rayx::xml::Parser parser) {
    Behavior behavior;
    rayx::xml::paramStr(parser.node, "figure", &behavior.figure);
    rayx::xml::paramStr(parser.node, "material", &behavior.material);
    rayx::xml::paramStr(parser.node, "coating", &behavior.coating);
    return behavior;
}

void makeOptionalCurvature(rayx::xml::Parser parser, SurfaceElement& se) {
    std::optional<Curvature> curvatureOpt;
    if (rayx::xml::param(parser.node, "curvature", &curvatureOpt)) {
        se.curvature(*curvatureOpt);
    }
}

void makeOptionalAperture(rayx::xml::Parser parser, SurfaceElement& se) {
    std::optional<Aperture> apertureOpt;
    if (rayx::xml::param(parser.node, "aperture", &apertureOpt)) {
        se.aperture(*apertureOpt);
    }
}

void makeOptionalSlopeError(rayx::xml::Parser parser, SurfaceElement& se) {
    std::optional<SlopeError> slopeErrorOpt;
    if (rayx::xml::param(parser.node, "slopeError", &slopeErrorOpt)) {
        se.slopeError(*slopeErrorOpt);
    }
}

}