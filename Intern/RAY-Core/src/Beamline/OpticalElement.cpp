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
    m_azimuthalAngle = defaultAzimuthalAngle(dobj);
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

double defaultMaterial(const DesignObject& dobj) { return (double)static_cast<int>(dobj.parseMaterial()); }

Rad defaultAzimuthalAngle(const DesignObject& dobj) {
    Rad azim;

    // TODO(Rudi) replace try-catch stuff by std::optionals
    try {
        azim = dobj.parseAzimuthalAngle();
    } catch (std::runtime_error& e) {
        azim.rad = 0;
    }
    return azim;
}

Surface makePlane() {
    return serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 0,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 0,
        .m_a23 = 0,
        .m_a24 = -1,
        .m_a33 = 0,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

Surface makeSphere(double radius) {
    return serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 1,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = -radius,
        .m_a33 = 1,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

Surface makeToroid(const DesignObject& dobj) {
    return serializeToroid({
        .m_longRadius = dobj.parseLongRadius(),
        .m_shortRadius = dobj.parseShortRadius(),
    });
}

Behaviour makeGrating(const DesignObject& dobj) {
    auto vls = dobj.parseVls();
    return serializeGrating({
        .m_vls = {vls[0], vls[1], vls[2], vls[3], vls[4], vls[5]},
        .m_lineDensity = dobj.parseLineDensity(),
        .m_orderOfDiffraction = dobj.parseOrderDiffraction(),
    });
}

Element makeElement(const DesignObject& dobj, Behaviour behaviour, Surface surface) {
    auto slopeError = dobj.parseSlopeError();

    return Element{
        .m_inTrans = defaultInMatrix(dobj),
        .m_outTrans = defaultOutMatrix(dobj),
        .m_behaviour = behaviour,
        .m_surface = surface,
        .m_cutout = dobj.parseCutout(),
        .m_slopeError = {slopeError[0], slopeError[1], slopeError[2], slopeError[3], slopeError[4], slopeError[5], slopeError[6]},
        .m_azimuthalAngle = defaultAzimuthalAngle(dobj).rad,
        .m_material = defaultMaterial(dobj),
        .m_padding = {0.0},
    };
}

}  // namespace RAYX
