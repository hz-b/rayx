#ifndef APPLY_SLOPE_ERROR_H
#define APPLY_SLOPE_ERROR_H

#include "Common.h"
#include "InvocationState.h"
#include "Rand.h"

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
RAYX_FUNC dvec3 RAYX_API normalCartesian(dvec3 normal, double x_rad, double z_rad);

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
RAYX_FUNC dvec3 RAYX_API normalCylindrical(dvec3 normal, double x_rad, double z_rad);

RAYX_FUNC_INLINE
double deg2rad(double degree) {
    return PI * degree / 180;
}

/**
adds slope error to the normal
@params:
    normal: normal vector
    error: the slope error to apply
    O_type: cartesian or cylindrical type of slope error (0=cartesian,
1=cylindrical) (1 only for ellipsis relevant) returns new normal if there is a
slope error in either x or z direction or the unmodified normal otherwise.
*/
template <typename MemSpace>
RAYX_FUNC_INLINE
dvec3 applySlopeError(dvec3 normal, SlopeError error, int O_type, Inv<MemSpace>& inv) {
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

#endif
