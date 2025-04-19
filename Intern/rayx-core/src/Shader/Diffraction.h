#pragma once

#include "Core.h"
#include "InvocationState.h"

namespace RAYX {

struct Rand;

// Calculates the factorial of n: n!
RAYX_FN_ACC double RAYX_API fact(int n);

/**returns first bessel function of parameter v*/
RAYX_FN_ACC double RAYX_API bessel1(double v);

/**
calculates the Bessel diffraction effects on circular slits and on circular
zoneplates
@params:	radius		radius < 0 (mm)
            wl			wavelength (nm)
            dphi, dpsi  angles of diffracted ray
@returns
    results stored in dphi, dpsi (inout)
*/
RAYX_FN_ACC void bessel_diff(double radius, double wl, double& dphi, double& dpsi, Rand& rand);

/**
 * calculates fraunhofer diffraction effects on rectangular slits
 * @param dim		dimension (x or y) (mm)
 * @param wl			wavelength (nm)
 * @param dAngle 	diffraction angle (inout)
 * @return result stored in dAngle
 */
RAYX_FN_ACC void fraun_diff(double dim, double wl, double& dAngle, Rand& rand);

}  // namespace RAYX
