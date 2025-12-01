#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Rotation.h"

namespace rayx {

struct Object {
    std::string name = createUniqueObjectName();
    glm::dvec3 position;
    Rotation rotation;
    // DesignPlane designPlane = DesignPlane::XZ; // TODO: rethink
};

}  // namespace rayx
