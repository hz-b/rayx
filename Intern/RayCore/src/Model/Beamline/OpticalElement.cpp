#include "OpticalElement.h"

#include <glm.hpp>
#include <optional>

#include "Debug/Debug.h"

namespace RAYX {

OpticalElement::OpticalElement(const DesignObject& dobj) {
    m_name = dobj.name();
    m_slopeError = dobj.parseSlopeError();
    m_material = dobj.parseMaterial();

    auto [cutoutTy, cutoutParams] = dobj.parseCutout();
    m_cutoutType = cutoutTy;
    m_cutoutParams = cutoutParams;

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
        .m_type = (double)getElementType(),
        .m_elementParams = {0},  // initialized below
        .m_surfaceType = (double)m_surfaceType,
        .m_surfaceParams = {0},  // initialized below
        .m_cutoutType = (double)m_cutoutType,
        .m_cutoutParams = {m_cutoutParams[0], m_cutoutParams[1], m_cutoutParams[2]},
        .m_slopeError = {m_slopeError[0], m_slopeError[1], m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5], m_slopeError[6]},
        .m_azimuthalAngle = m_azimuthalAngle.rad,
        .m_material = (double)static_cast<int>(m_material),
        .m_padding = {0.0},
    };
    for (int i = 0; i < 16; i++) {
        e.m_elementParams[i] = getElementParams().data()[i];
        e.m_surfaceParams[i] = m_surfaceParams.data()[i];
    }

    return e;
}

/**
 * calculates element to world coordinates transformation matrix and its
 * inverse
 * @param   position     4 element vector which describes the position
 * of the element in world coordinates
 * @param   orientation  4x4 matrix that describes the orientation of
 * the surface with respect to the world coordinate system
 * @return void
 */
void OpticalElement::calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, glm::dmat4& output, bool calcInMatrix) const {
    glm::dmat4x4 rotation =
        glm::dmat4x4(orientation[0][0], orientation[0][1], orientation[0][2], 0.0, orientation[1][0], orientation[1][1], orientation[1][2], 0.0,
                     orientation[2][0], orientation[2][1], orientation[2][2], 0.0, 0.0, 0.0, 0.0, 1.0);  // o
    glm::dmat4x4 inv_rotation = glm::transpose(rotation);

    if (calcInMatrix) {
        glm::dmat4x4 translation = glm::dmat4x4(1, 0, 0, -position[0], 0, 1, 0, -position[1], 0, 0, 1, -position[2], 0, 0, 0, 1);  // o
        // ray = tran * rot * ray
        glm::dmat4x4 g2e = translation * rotation;
        output = glm::transpose(g2e);
        return;
    } else {
        glm::dmat4x4 inv_translation = glm::dmat4x4(1, 0, 0, position[0], 0, 1, 0, position[1], 0, 0, 1, position[2], 0, 0, 0, 1);  // o
        // inverse of m_inMatrix
        glm::dmat4x4 e2g = inv_rotation * inv_translation;
        output = glm::transpose(e2g);
        return;
    }
}

glm::dmat4 OpticalElement::getInMatrix() const {
    // return glmToArray16(m_inMatrix);,
    glm::dmat4 inMatrix = glm::dmat4();
    calcTransformationMatrices(m_position, m_orientation, inMatrix);
    return inMatrix;
}
glm::dmat4 OpticalElement::getOutMatrix() const {
    // return glmToArray16(m_outMatrix);
    glm::dmat4 outMatrix = glm::dmat4();
    calcTransformationMatrices(m_position, m_orientation, outMatrix, false);
    return outMatrix;
}
glm::dvec4 OpticalElement::getPosition() const { return m_position; }
glm::dmat4x4 OpticalElement::getOrientation() const { return m_orientation; }

std::array<double, 7> OpticalElement::getSlopeError() const { return m_slopeError; }

std::array<double, 16> OpticalElement::getElementParams() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
