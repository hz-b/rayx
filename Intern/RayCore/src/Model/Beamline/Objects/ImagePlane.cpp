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
    : OpticalElement(name, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}) {
    m_Geometry->m_orientation = orientation;
    m_Geometry->m_position = position;

    setSurface(std::make_unique<Quadric>(std::array<double, 4 * 4>{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0}));
}
ImagePlane::~ImagePlane() = default;

std::shared_ptr<ImagePlane> ImagePlane::createFromXML(const xml::Parser& p) {
    return std::make_shared<ImagePlane>(p.name(), p.parsePosition(),
                                        p.parseOrientation());
}

std::array<double, 4*4> ImagePlane::getElementParameters() const {
    return {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
}
}  // namespace RAYX
