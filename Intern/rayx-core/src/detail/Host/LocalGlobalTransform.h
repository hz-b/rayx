#pragma once

#include <glm/glm.hpp>

#include "Core.h"
#include "Design/Beamline.h"

namespace rayx::detail::host {

struct RAYX_API LocalGlobalTransform {
    glm::dmat4 globalToLocal;
    glm::dmat4 localToGlobal;
};

// /**
//  * @brief Computes the local-to-global and global-to-local transformation matrices for a given position and rotation.
//  *
//  * This function constructs the transformation matrices based on the provided position and rotation.
//  * It also adjusts the matrices according to the specified design plane (XY or XZ).
//  *
//  * @param position The position vector of the element in world coordinates.
//  * @param rotation The rotation of the element represented as a Rotation object.
//  * @param plane The design plane (XY or XZ) that determines how the matrices are adjusted.
//  * @return A LocalGlobalTransform struct containing both the global-to-local and local-to-global transformation matrices.
//  */
// RAYX_API LocalGlobalTransform getLocalGlobalTransform(const glm::dvec3& position, const Rotation& rotation, const DesignPlane plane);
//
// /**
//  * @brief Computes the local-to-global and global-to-local transformation matrices for a given BeamlineNode.
//  *
//  * This function retrieves the position and rotation from the BeamlineNode and determines the design plane
//  * based on the source or element. It then calls the overloaded getLocalGlobalTransform
//  * function to compute the transformation matrices.
//  *
//  * @param node The BeamlineNode for which to compute the transformation matrices.
//  * @return A LocalGlobalTransform struct containing both the global-to-local and local-to-global transformation matrices.
//  */
// RAYX_API LocalGlobalTransform getLocalGlobalTransform(const Beamline& node);

}  // namespace rayx::detail::host
