#include "OpticalElement.h"

#include <glm.hpp>
#include <optional>

#include "Debug/Debug.h"

namespace RAYX {

OpticalElement::OpticalElement(const DesignObject& dobj) {
    m_name = dobj.name();
    m_slopeError = dobj.parseSlopeError();
    m_material = dobj.parseMaterial();

    m_cutout = dobj.parseCutout();

    // TODO(Rudi) replace try-catch stuff by std::optionals
    try {
        m_azimuthalAngle = dobj.parseAzimuthalAngle();
    } catch (std::runtime_error& e) {
    }

    m_position = dobj.parsePosition();
    m_orientation = dobj.parseOrientation();
}

Element OpticalElement::intoElement() const {
    Element e = Element{
        .m_inTrans = getInMatrix(),
        .m_outTrans = getOutMatrix(),
        .m_behaviour = m_behaviour,
        .m_surface = m_surface,
        .m_cutout = m_cutout,
        .m_slopeError = {m_slopeError[0], m_slopeError[1], m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5], m_slopeError[6]},
        .m_azimuthalAngle = m_azimuthalAngle.rad,
        .m_material = (double)static_cast<int>(m_material),
        .m_padding = {0.0},
    };

    return e;
}

glm::dmat4 OpticalElement::getInMatrix() const { return calcTransformationMatrices(m_position, m_orientation); }
glm::dmat4 OpticalElement::getOutMatrix() const { return calcTransformationMatrices(m_position, m_orientation, false); }
glm::dvec4 OpticalElement::getPosition() const { return m_position; }
glm::dmat4x4 OpticalElement::getOrientation() const { return m_orientation; }

std::array<double, 7> OpticalElement::getSlopeError() const { return m_slopeError; }

/**
 * calculates element to world coordinates transformation matrix and its
 * inverse
 * @param   position     4 element vector which describes the position
 * of the element in world coordinates
 * @param   orientation  4x4 matrix that describes the orientation of
 * the surface with respect to the world coordinate system
 * @return the in or out matrix according to `calcInMatrix`
 */
glm::dmat4 calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, bool calcInMatrix) {
    glm::dmat4x4 rotation =
        glm::dmat4x4(orientation[0][0], orientation[0][1], orientation[0][2], 0.0, orientation[1][0], orientation[1][1], orientation[1][2], 0.0,
                     orientation[2][0], orientation[2][1], orientation[2][2], 0.0, 0.0, 0.0, 0.0, 1.0);  // o
    glm::dmat4x4 inv_rotation = glm::transpose(rotation);

    if (calcInMatrix) {
        glm::dmat4x4 translation = glm::dmat4x4(1, 0, 0, -position[0], 0, 1, 0, -position[1], 0, 0, 1, -position[2], 0, 0, 0, 1);  // o
        // ray = tran * rot * ray
        glm::dmat4x4 g2e = translation * rotation;
        return glm::transpose(g2e);
    } else {
        glm::dmat4x4 inv_translation = glm::dmat4x4(1, 0, 0, position[0], 0, 1, 0, position[1], 0, 0, 1, position[2], 0, 0, 0, 1);  // o
        // inverse of m_inMatrix
        glm::dmat4x4 e2g = inv_rotation * inv_translation;
        return glm::transpose(e2g);
    }
}

glm::dmat4x4 defaultInMatrix(const DesignObject& dobj) { return calcTransformationMatrices(dobj.parsePosition(), dobj.parseOrientation(), true); }

glm::dmat4x4 defaultOutMatrix(const DesignObject& dobj) { return calcTransformationMatrices(dobj.parsePosition(), dobj.parseOrientation(), false); }

}  // namespace RAYX
