#ifndef UTILS_H
#define UTILS_H

#include "Adapt.h"
#include "Ray.h"

// converts energy in eV to wavelength in nm
double RAYX_API hvlam(double x);

double infinity();

// multiplies position and direction of ray r with transformation matrix m
// r = dot(m, r)
Ray RAYX_API rayMatrixMult(Ray r, const dmat4 m);

// returns angle between ray direction and surface normal at intersection point
double RAYX_API getIncidenceAngle(Ray r, dvec3 normal);

#endif
