#pragma once

#include "Common.h"
#include "InvocationState.h"

namespace RAYX {

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: line spacing in z direction
            ax: line spacing in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
RAYX_FUNC Ray refrac2D(Ray r, dvec3 normal, double az, double ax, Inv& inv);

// refraction function used for gratings
RAYX_FUNC Ray refrac(Ray r, dvec3 normal, double linedensity, Inv& inv);

// TODO the function refracPlane is not currently in use!
/*
 * simplified refraction function used for plane gratings
 * normal is always the same (0,1,0) -> no rotation and thus no trigonometric
 * functions necessary
 * @param r			ray
 * @param normal		normal at ray-object intersection (for planes always
 * (0,1,0))
 * @param a			a = WL * D0 * ORD * 1.e-6  with D0: line density (l/mm);
 * WL:wavelength (nm); ORD order of diffraction
 */
RAYX_FUNC Ray RAYX_API refracPlane(Ray r, [[maybe_unused]] dvec3 normal,
                          double a, Inv& inv);  // TODO fix unused var

} // namespace RAYX
