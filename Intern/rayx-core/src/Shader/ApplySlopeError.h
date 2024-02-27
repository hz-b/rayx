#ifndef APPLY_SLOPE_ERROR_H
#define APPLY_SLOPE_ERROR_H

#include "Common.h"
#include "InvocationState.h"

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
dvec3 RAYX_API normalCartesian(dvec3 normal, double x_rad, double z_rad);

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
dvec3 RAYX_API normalCylindrical(dvec3 normal, double x_rad, double z_rad);

/**
adds slope error to the normal
@params:
    normal: normal vector
    error: the slope error to apply
    O_type: cartesian or cylindrical type of slope error (0=cartesian,
1=cylindrical) (1 only for ellipsis relevant) returns new normal if there is a
slope error in either x or z direction or the unmodified normal otherwise.
*/
dvec3 applySlopeError(dvec3 normal, SlopeError error, int O_type, Inv& inv);

#endif
