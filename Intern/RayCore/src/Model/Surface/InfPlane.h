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

    inline std::array<double, 16> getParams() const { return std::array<double, 16>{0}; }
};
}  // namespace RAYX
