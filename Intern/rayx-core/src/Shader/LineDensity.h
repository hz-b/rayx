#pragma once

#include "Core.h"
#include "Element/Behaviour.h"
#include "Ray.h"

namespace RAYX {

/** variable line spacing for gratings
@params
    lineDensity: general line density?
    z: z-coordinate of ray position
    vls[6]: 6 vls parameters given by user
@returns line density specifically for this z-coordinate
*/
RAYX_FN_ACC double RAYX_API vlsGrating(double lineDensity, glm::dvec3 normal, double z, const double vls[6]);

/**
calculates DX and DZ (line spacing in x and z direction) at a given point for a
given direction on the grating
@params: lots
@returns: (inplace) DX, DZ
*/
RAYX_FN_ACC void RAYX_API RZPLineDensity(Ray r, glm::dvec3 normal, Behaviour::RZP b, double& DX, double& DZ);

}  // namespace RAYX
