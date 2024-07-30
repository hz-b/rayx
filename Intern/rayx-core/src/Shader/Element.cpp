#include "Element.h"

#include "DesignElement/DesignElement.h"

namespace RAYX {

// No additional implementation needed for now since the structs do not have member functions

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
        1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
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

inline glm::dmat4x4 defaultInMatrix(const DesignElement& dele, DesignPlane plane) {
    return calcTransformationMatrices(dele.getWorldPosition(), dele.getWorldOrientation(), true, plane);
}

inline glm::dmat4x4 defaultOutMatrix(const DesignElement& dele, DesignPlane plane) {
    return calcTransformationMatrices(dele.getWorldPosition(), dele.getWorldOrientation(), false, plane);
}

inline double defaultMaterial(const DesignElement& dele) { return (double)static_cast<int>(dele.getMaterial()); }

Element makeElement(const DesignElement& dele, Behaviour behaviour, Surface surface, std::optional<Cutout> cutout, DesignPlane plane) {
    if (!cutout) {
        cutout = dele.getCutout();
    }

    auto inMat = defaultInMatrix(dele, plane);
    auto outMat = defaultOutMatrix(dele, plane);

    return Element{
        .m_inTrans = inMat,
        .m_outTrans = outMat,
        .m_behaviour = behaviour,
        .m_surface = surface,
        .m_cutout = *cutout,
        .m_slopeError = dele.getSlopeError(),
        .m_azimuthalAngle = dele.getAzimuthalAngle().rad,
        .m_material = defaultMaterial(dele),
    };
}
}  // namespace RAYX
