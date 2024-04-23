#pragma once

#include "Common.h"
#include "Ray.h"

namespace RAYX {

//TODO: doku
RAYX_FUNC dvec3 RAYX_API cubicPosition(Ray r, double alpha);

RAYX_FUNC dvec3 RAYX_API cubicDirection(Ray r, double alpha);

} // namespace RAYX
