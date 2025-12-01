#include "Element.h"

#include <algorithm>

#include "Design/DesignElement.h"

namespace rayx {

LocalGlobalTransform getLocalGlobalTransform(const glm::dvec3& position, const Rotation& rotation, const DesignPlane plane) {
    const auto translation = glm::translate(glm::dmat4(1.0), -position);
    const auto rotation4x4 = glm::dmat4(toRotationMatrix(rotation));

    auto globalToLocal = rotation4x4 * translation;
    auto localToGlobal = glm::transpose(rotation4x4) * glm::translate

    if (plane == DesignPlane::XY) {
        const glm::dmat4x4 yz_swap = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, };
        globalToLocal = yz_swap * globalToLocal;
        localToGlobal = localToGlobal * yz_swap;
    }

    return {
        .globalToLocal = globalToLocal,
        .localToGlobal = localToGlobal,
    };
}

LocalGlobalTransform getLocalGlobalTransform(const BeamlineNode& node) {
    if (node.isSource()) {
        return getLocalGlobalTransform(node.position, node.rotation, getDesignPlane(node.toSource()->source));
    } else if (node.isElement()) {
        return getLocalGlobalTransform(node.position, node.rotation, getDesignPlane(node.toElement()->element));
    } else {
        RAYX_EXIT << "Cannot get LocalGlobalTransform for BeamlineNode that is neither Source nor Element.";
        return {};
    }
}

}  // namespace rayx
