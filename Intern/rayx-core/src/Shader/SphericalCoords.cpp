#include "SphericalCoords.h"
#include "Approx.h"

namespace RAYX {

RAYX_FUNC
void RAYX_API sphericalCoordsToDirection(double phi, double psi, dvec3& out_direction) {
    double cos_psi = r8_cos(psi);
    double cos_phi = r8_cos(phi);
    double sin_psi = r8_sin(psi);
    double sin_phi = r8_sin(phi);

    out_direction = dvec3(cos_psi * sin_phi, cos_psi * cos_phi, -sin_psi);
}

RAYX_FUNC
void RAYX_API directionToSphericalCoords(dvec3 direction, double& out_phi, double& out_psi) {
    out_psi = -r8_asin(direction.z);
    out_phi = r8_atan2(direction.x, direction.y);
}

} // namespace RAYX
