#include "SphericalCoords.h"

namespace RAYX {

RAYX_FN_ACC
void RAYX_API sphericalCoordsToDirection(double phi, double psi, glm::dvec3& out_direction) {
    double cos_psi = glm::cos(psi);
    double cos_phi = glm::cos(phi);
    double sin_psi = glm::sin(psi);
    double sin_phi = glm::sin(phi);

    out_direction = glm::dvec3(cos_psi * sin_phi, cos_psi * cos_phi, -sin_psi);
}

RAYX_FN_ACC
void RAYX_API directionToSphericalCoords(glm::dvec3 direction, double& out_phi, double& out_psi) {
    out_psi = -glm::asin(direction.z);
    out_phi = glm::atan(direction.x, direction.y);
}

}  // namespace RAYX
