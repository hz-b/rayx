#pragma once

#include "Common.h"

namespace RAYX {

// Consider this https://en.wikipedia.org/wiki/Spherical_coordinate_system.
// But we fix the radius r = 1, as we only care to express direction, not distance.

// The definitions of phi & psi might differ from their definitions at other places, due to the fact that we exchanged y with z while fixing XZ as the
// primary plane in the shader.

RAYX_FN_ACC void RAYX_API sphericalCoordsToDirection(double phi, double psi, dvec3& out_direction);
RAYX_FN_ACC void RAYX_API directionToSphericalCoords(dvec3 direction, double& out_phi, double& out_psi);

}  // namespace RAYX
