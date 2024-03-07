#include "ApplySlopeError.h"
#include "Approx.h"

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
RAYX_FUNC
dvec3 RAYX_API normalCartesian(dvec3 normal, double x_rad, double z_rad) {
    double FX = normal[0];
    double FY = normal[1];
    double FZ = normal[2];

    double cosx = r8_cos(x_rad);
    double sinx = r8_sin(x_rad);
    double cosz = r8_cos(z_rad);
    double sinz = r8_sin(z_rad);

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
RAYX_FUNC
dvec3 RAYX_API normalCylindrical(dvec3 normal, double x_rad, double z_rad) {
    double normFXFY = sqrt(normal[0] * normal[0] + normal[1] * normal[1]);
    double arcTanFXFY = r8_atan2(normal[1], normal[0]);
    double sinz = r8_sin(z_rad);
    double cosz = r8_cos(z_rad);

    normal[0] = r8_cos(x_rad + arcTanFXFY) * (normFXFY * cosz + normal[2] * sinz);
    normal[1] = r8_sin(x_rad + arcTanFXFY) * (normFXFY * cosz + normal[2] * sinz);
    normal[2] = normal[2] * cosz - normFXFY * sinz;

    return normal;
}
