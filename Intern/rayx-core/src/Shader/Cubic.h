#pragma once

#include "Core.h"
#include "Ray.h"

namespace RAYX {

// TODO: doku
RAYX_FN_ACC glm::dvec3 RAYX_API cubicPosition(Ray r, double alpha);

RAYX_FN_ACC glm::dvec3 RAYX_API cubicDirection(Ray r, double alpha);

}  // namespace RAYX
