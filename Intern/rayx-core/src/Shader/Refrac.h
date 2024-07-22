#ifndef REFRAC_H
#define REFRAC_H

#include "Adapt.h"

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: line spacing in z direction
            ax: line spacing in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
Ray refrac2D(Ray r, dvec3 normal, double az, double ax);

/*
 * simplified refraction function used for gratings
 * for planes normal is always the same (0,1,0) -> no rotation and thus no trigonometric
 * functions necessary TODO: set case for planes
 * @param r			ray
 * @param normal		normal at ray-object intersection (for planes always
 * (0,1,0))
 * @param a			a = WL * D0 * ORD * 1.e-6  with D0: line density (l/mm);
 * WL:wavelength (nm); ORD order of diffraction
 */
Ray refrac(Ray r, dvec3 normal, double linedensity);

#endif
