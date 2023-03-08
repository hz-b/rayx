#pragma once

#include <glm.hpp>

#include "Core.h"
#include "Surface.h"
#include "utils.h"

namespace RAYX {
class RAYX_API InfPlane : public Surface {
  public:
    // ctor
    InfPlane() = default;

    inline int getSurfaceType() const { return STY_INF_PLANE; }

    // dtor
    ~InfPlane() = default;

    inline glm::dmat4x4 getParams() const {
        glm::dmat4x4 x(0);
        return x;
    }
};
}  // namespace RAYX
