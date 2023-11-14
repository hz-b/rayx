#include "OpticalElement.h"

#include <glm.hpp>
#include <optional>

namespace RAYX {

glm::dmat4x4 defaultInMatrix(const DesignObject& dobj, DesignPlane plane) {
    return calcTransformationMatrices(dobj.parsePosition(), dobj.parseOrientation(), true, plane);
}
glm::dmat4x4 defaultOutMatrix(const DesignObject& dobj, DesignPlane plane) {
    return calcTransformationMatrices(dobj.parsePosition(), dobj.parseOrientation(), false, plane);
}

/**
 * calculates element to world coordinates transformation matrix and its
 * inverse
 * @param   position     4 element vector which describes the position
 * of the element in world coordinates
 * @param   orientation  4x4 matrix that describes the orientation of
 * the surface with respect to the world coordinate system
 * @return the in or out matrix according to `calcInMatrix`
 */
glm::dmat4 calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, bool calcInMatrix, DesignPlane plane) {
    glm::dmat4x4 rotation =
        glm::dmat4x4(orientation[0][0], orientation[0][1], orientation[0][2], 0.0, orientation[1][0], orientation[1][1], orientation[1][2], 0.0,
                     orientation[2][0], orientation[2][1], orientation[2][2], 0.0, 0.0, 0.0, 0.0, 1.0);  // o

    glm::dmat4x4 yz_swap = {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1,
    };

    glm::dmat4x4 inv_rotation = glm::transpose(rotation);

    if (calcInMatrix) {
        glm::dmat4x4 translation = glm::dmat4x4(1, 0, 0, -position[0], 0, 1, 0, -position[1], 0, 0, 1, -position[2], 0, 0, 0, 1);  // o
        // ray = tran * rot * ray
        glm::dmat4x4 g2e = translation * rotation;
        if (plane == DesignPlane::XY) g2e = g2e * yz_swap;
        return glm::transpose(g2e);
    } else {
        glm::dmat4x4 inv_translation = glm::dmat4x4(1, 0, 0, position[0], 0, 1, 0, position[1], 0, 0, 1, position[2], 0, 0, 0, 1);  // o
        // inverse of m_inMatrix
        glm::dmat4x4 e2g = inv_rotation * inv_translation;
        if (plane == DesignPlane::XY) e2g = yz_swap * e2g;
        return glm::transpose(e2g);
    }
}

double defaultMaterial(const DesignObject& dobj) { return (double)static_cast<int>(dobj.parseMaterial()); }

Surface makePlane() {
    return serializePlaneXZ();
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
        .m_toroidType = TOROID_TYPE_CONCAVE,
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

Element makeElement(const DesignObject& dobj, Behaviour behaviour, Surface surface, std::optional<Cutout> cutout, DesignPlane plane) {
    if (!cutout) {
        cutout = dobj.parseCutout(plane);
    }

    auto inMat = defaultInMatrix(dobj, plane);
    auto outMat = defaultOutMatrix(dobj, plane);

    return Element {
        .m_inTrans = inMat,
        .m_outTrans = outMat,
        .m_behaviour = behaviour,
        .m_surface = surface,
        .m_cutout = *cutout,
        .m_slopeError = dobj.parseSlopeError(),
        .m_azimuthalAngle = dobj.parseAzimuthalAngle().rad,
        .m_material = defaultMaterial(dobj),
        .m_padding = {0.0},
    };
}

Element makeExperts(const DesignObject& dobj) {
    return makeElement(dobj, serializeMirror(), makeQuadric(dobj));
}

Element makeExpertsCubic(const DesignObject& dobj) {
    return makeElement(dobj, serializeMirror(), makeCubic(dobj));
}

Surface makeQuadric(const DesignObject& dobj) {
    return serializeQuadric(dobj.parseQuadricParameters());
}

Surface makeCubic(const DesignObject& dobj) {
    return serializeCubic(dobj.parseCubicParameters());
}

}  // namespace RAYX
