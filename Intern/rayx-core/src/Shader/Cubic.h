#pragma once

#include "Core.h"
#include "Ray.h"

namespace RAYX {

// TODO: doku
RAYX_FN_ACC glm::dvec3 RAYX_API cubicPosition(const glm::dvec3& __restrict rayPosition, const double alpha);

RAYX_FN_ACC glm::dvec3 RAYX_API cubicDirection(const glm::dvec3& __restrict rayDirection, double alph);

}  // namespace RAYX
