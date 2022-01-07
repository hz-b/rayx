#include "ImagePlane.h"

#include <Data/xml.h>

#include "Debug.h"

namespace RAYX {

/**
 * initializes transformation matrices from position and orientation, and sets
 * parameters for the quadric in super class (optical Element)
 * @param   name            name of ImagePlane
 * @param   position        distance to preceeding element
 * @param   orientation     pointer to previous element in beamline, needed for
 * calculating world coordinates
 * does not have an azimuthal angle
 */
ImagePlane::ImagePlane(const char* name, glm::dvec4 position,
                       glm::dmat4x4 orientation)
    : OpticalElement(name, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     Geometry::GeometricalShape::RECTANGLE, 0, 0, 0, position,
                     orientation, {0, 0, 0, 0, 0, 0, 0}) {
    RAYX_LOG << name;
    setSurface(std::make_unique<Quadric>(
        std::vector<double>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0}));
}

ImagePlane::~ImagePlane() {}

std::shared_ptr<ImagePlane> ImagePlane::createFromXML(
    rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context) {
    const char* name = node->first_attribute("name")->value();

    glm::dvec4 position;
    glm::dmat4x4 orientation;
    if (!xml::paramPositionAndOrientation(node, group_context, &position,
                                          &orientation)) {
        return nullptr;
    }

    return std::make_shared<ImagePlane>(name, position, orientation);
}

}  // namespace RAYX
