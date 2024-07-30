#include "ApplySlopeError.h"

#include "Constants.h"
#include "Rand.h"

namespace RAYX {

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
RAYX_FN_ACC
glm::dvec3 RAYX_API normalCartesian(glm::dvec3 normal, double x_rad, double z_rad) {
    double FX = normal[0];
    double FY = normal[1];
    double FZ = normal[2];

    double cosx = glm::cos(x_rad);
    double sinx = glm::sin(x_rad);
    double cosz = glm::cos(z_rad);
    double sinz = glm::sin(z_rad);

    // put in matrix mult?
    double FY2 = FY * cosz + FZ * sinz;
    normal[0] = FX * cosx + FY2 * sinx;
    normal[1] = FY2 * cosx - FX * sinx;
    normal[2] = FZ * cosz - FY * sinz;

    return normal;
}

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
RAYX_FN_ACC
glm::dvec3 RAYX_API normalCylindrical(glm::dvec3 normal, double x_rad, double z_rad) {
    double normFXFY = sqrt(normal[0] * normal[0] + normal[1] * normal[1]);
    double arcTanFXFY = glm::atan(normal[1], normal[0]);
    double sinz = glm::sin(z_rad);
    double cosz = glm::cos(z_rad);

    normal[0] = glm::cos(x_rad + arcTanFXFY) * (normFXFY * cosz + normal[2] * sinz);
    normal[1] = glm::sin(x_rad + arcTanFXFY) * (normFXFY * cosz + normal[2] * sinz);
    normal[2] = normal[2] * cosz - normFXFY * sinz;

    return normal;
}

RAYX_FN_ACC
double deg2rad(double degree) { return PI * degree / 180; }

/**
adds slope error to the normal
@params:
    normal: normal vector
    error: the slope error to apply
    O_type: cartesian or cylindrical type of slope error (0=cartesian,
1=cylindrical) (1 only for ellipsis relevant) returns new normal if there is a
slope error in either x or z direction or the unmodified normal otherwise.
*/
RAYX_FN_ACC
glm::dvec3 applySlopeError(glm::dvec3 normal, SlopeError error, int O_type, InvState& inv) {
    double slopeX = error.m_sag;
    double slopeZ = error.m_mer;

    // only calculate the random number if at least one slope error is not 0,
    // since the calculation is costly (sin, cos, log involved)
    if (slopeX != 0 || slopeZ != 0) {
        double random_values[2] = {squaresNormalRNG(inv.ctr, 0, slopeX), squaresNormalRNG(inv.ctr, 0, slopeZ)};

        /*double x = random_values[0] * slopeX; // to get normal distribution
        from std.-norm. multiply by sigma (=slopeX) -> mu + x * sigma but mu=0
        here double z = random_values[1] * slopeZ;*/

        double x_rad = deg2rad(random_values[0] / 3600.0);
        double z_rad = deg2rad(random_values[1] / 3600.0);

        if (O_type == 0) {  // default
            normal = normalCartesian(normal, x_rad, z_rad);
        } else if (O_type == 1) {
            normal = normalCylindrical(normal, x_rad, z_rad);
        }
    }

    return normal;
}

}  // namespace RAYX
