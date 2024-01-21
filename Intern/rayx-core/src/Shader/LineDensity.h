#ifndef RZP_LINE_DENSITY_H
#define RZP_LINE_DENSITY_H

#include "Adapt.h"

/** variable line spacing for gratings
@params
    lineDensity: general line density?
    z: z-coordinate of ray position
    vls[6]: 6 vls parameters given by user
@returns line density specifically for this z-coordinate
*/
double RAYX_API vlsGrating(double lineDensity, double z, double vls[6]);

/**
calculates DX and DZ (line spacing in x and z direction) at a given point for a
given direction on the grating
@params: lots
@returns: (inplace) DX, DZ
*/
void RAYX_API RZPLineDensity(Ray r, dvec3 normal, RZPBehaviour b, RAYX_INOUT(double) DX, RAYX_INOUT(double) DZ);

#endif
