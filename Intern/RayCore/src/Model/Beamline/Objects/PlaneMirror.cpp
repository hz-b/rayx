#include "PlaneMirror.h"

#include <Data/xml.h>

#include "Debug.h"

namespace RAYX {

/**
 * initializes transformation matrices from position and orientation, and
 * parameters for the quadric in super class (optical element) sets
 * mirror-specific parameters in this class
 * @param name                  name of the plane mirror
 * @param width                 width of mirror (x-dimension in element coord.
 * sys.)
 * @param height                height of mirror (z-dimension in element coord.
 * sys.)
 * @param azimuthalAngle        azimuthal angle of object (rotation in xy plane
 * with respect to previous element) in rad
 * @param position              position in world coordinates (in homogeneous
 * coordinates)
 * @param orientation           orientation in world coordinates
 * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z
 * meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing
 * amplitude y(5) and radius (6)
 * @param mat                   material (See Material.h)
 *
 */
PlaneMirror::PlaneMirror(const char* name,
                         Geometry::GeometricalShape geometricalShape,
                         const double width, const double height,
                         const double azimuthalAngle, glm::dvec4 position,
                         glm::dmat4x4 orientation,
                         const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     geometricalShape, width, height, azimuthalAngle, position,
                     orientation, slopeError) {
    RAYX_LOG << name;
    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, matd, 0}));
}

PlaneMirror::PlaneMirror(const char* name,
                         Geometry::GeometricalShape geometricalShape,
                         const double width, const double widthB,
                         const double height, const double azimuthalAngle,
                         glm::dvec4 position, glm::dmat4x4 orientation,
                         const std::array<double, 7> slopeError, Material mat)
    : OpticalElement(name, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     geometricalShape, width, widthB, height, azimuthalAngle,
                     position, orientation, slopeError) {
    RAYX_LOG << name;
    double matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, matd, 0}));
}

PlaneMirror::~PlaneMirror() {}

std::shared_ptr<PlaneMirror> PlaneMirror::createFromXML(
    rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context) {
    const char* name = node->first_attribute("name")->value();

    int gs;
    if (!xml::paramInt(node, "geometricalShape", &gs)) {
        return nullptr;
    }
    Geometry::GeometricalShape geometricalShape =
        static_cast<Geometry::GeometricalShape>(
            gs);  // HACK(Jannis): convert to enum

    double width;
    if (!xml::paramDouble(node, "totalWidth", &width)) {
        return nullptr;
    }

    double height;
    if (!xml::paramDouble(node, "totalLength", &height)) {
        return nullptr;
    }

    glm::dvec4 position;
    glm::dmat4x4 orientation;
    if (!xml::paramPositionAndOrientation(node, group_context, &position,
                                          &orientation)) {
        return nullptr;
    }

    std::array<double, 7> slopeError;
    if (!xml::paramSlopeError(node, &slopeError)) {
        return nullptr;
    }

    double azimuthalAngle;
    if (!xml::paramDouble(node, "azimuthalAngle", &azimuthalAngle)) {
        return nullptr;
    }

    Material mat;
    if (!xml::paramMaterial(node, &mat)) {
        mat = Material::CU;  // default to copper
    }

    double widthB;
    bool foundWidthB = xml::paramDouble(node, "totalWidthB", &widthB);
    if (foundWidthB) {
        return std::make_shared<PlaneMirror>(
            name, geometricalShape, width, widthB, height,
            degToRad(azimuthalAngle), position, orientation, slopeError, mat);
    } else {
        return std::make_shared<PlaneMirror>(
            name, geometricalShape, width, height, degToRad(azimuthalAngle),
            position, orientation, slopeError, mat);
    }
}

}  // namespace RAYX
