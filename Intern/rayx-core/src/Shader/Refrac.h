#pragma once

#include <cmath>

#include "Complex.h"
#include "Ray.h"
#include <cmath>
#include "Complex.h" 
#include "Utils.h"

namespace RAYX {

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element -> for planes normal is always the same (0,1,0) -> no need to rotate but we do
anyways. az: linedensity in z direction ax: linedensity in x direction
@returns: refracted ray (position unchanged, direction changed), weight = EventType::BeyondHorizon if
"ray beyond horizon"
*/
RAYX_FN_ACC Ray refrac2D(Ray r, glm::dvec3 normal, double az, double ax);

/**
 * Simple refraction for double precision vectors
 * @params:  I: incident vector
 *           N: normal vector at the point of intersection
 *           eta: ratio of refractive indices (n1/n2)
 * @returns: refracted vector, or zero vector if total internal reflection occurs
 */
RAYX_FN_ACC glm::dvec3 refract_dvec3(glm::dvec3 I, glm::dvec3 N, double eta);

}  // namespace RAYX