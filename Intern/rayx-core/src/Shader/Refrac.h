#pragma once

#include "Ray.h"
#include "Utils.h"

namespace RAYX {

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element -> for planes normal is always the same (0,1,0) -> no need to rotate but we do
anyways. az: linedensity in z direction ax: linedensity in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
RAYX_FN_ACC Ray refrac2D(Ray r, glm::dvec3 normal, double az, double ax);

}  // namespace RAYX
