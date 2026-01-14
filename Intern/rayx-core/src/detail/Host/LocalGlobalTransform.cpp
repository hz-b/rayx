#include "LocalGlobalTransform.h"

#include <algorithm>

namespace rayx::detail::host {

// TODO: implement using TranslateNode and RotateNode

// LocalGlobalTransform getLocalGlobalTransform(const glm::dvec3& position, const Rotation& rotation, const DesignPlane plane) {
//     const auto translationLocalToGlobal = glm::translate(glm::dmat4(1.0), position);
//     const auto translationGlobalToLocal = glm::translate(glm::dmat4(1.0), -position);
//     const auto rotationLocalToGlobal    = glm::dmat4(toRotationMatrix(rotation));
//     const auto rotationGlobalToLocal    = glm::transpose(rotationLocalToGlobal);  // inverse of rotation matrix is its transpose
//
//     auto globalToLocal = rotationGlobalToLocal * translationGlobalToLocal;
//     auto localToGlobal = rotationLocalToGlobal * translationLocalToGlobal;
//
//     if (plane == DesignPlane::XY) {
//         const glm::dmat4x4 yz_swap = {
//             1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
//         };
//         globalToLocal = yz_swap * globalToLocal;
//         localToGlobal = localToGlobal * yz_swap;
//     }
//
//     return {
//         .globalToLocal = globalToLocal,
//         .localToGlobal = localToGlobal,
//     };
// }
//
// LocalGlobalTransform getLocalGlobalTransform(const Beamline& beamline, const BeamlineNode& node) {
//     DesignPlane designPlane;
//
//     if (node.isSource())
//         designPlane = getDesignPlane(node.toSource()->source);
//     else if (node.isElement())
//         designPlane = getDesignPlane(node.toElement()->element);
//     else {
//         RAYX_EXIT << "Cannot get LocalGlobalTransform for BeamlineNode that is neither Source nor Element.";
//         return {};
//     }
//
//     return getLocalGlobalTransform(node.getWorldSpacePosition(), node.getWorldSpaceRotation(), designPlane);
// }

}  // namespace rayx::detail::host
