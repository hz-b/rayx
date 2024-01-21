#ifndef SPHERICAL_COORDS_H
#define SPHERICAL_COORDS_H

#include "Adapt.h"

// Consider this https://en.wikipedia.org/wiki/Spherical_coordinate_system.
// But we fix the radius r = 1, as we only care to express direction, not distance.

// The definitions of phi & psi might differ from their definitions at other places, due to the fact that we exchanged y with z while fixing XZ as the primary plane in the shader.

void RAYX_API sphericalCoordsToDirection(double phi, double psi, RAYX_INOUT(dvec3) out_direction);
void RAYX_API directionToSphericalCoords(dvec3 direction, RAYX_INOUT(double) out_phi, RAYX_INOUT(double) out_psi);

#endif
