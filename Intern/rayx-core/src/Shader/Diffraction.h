#pragma once

#include "Common.h"
#include "InvocationState.h"

namespace RAYX {

// Calculates the factorial of n: n!
RAYX_FUNC double RAYX_API fact(int n);

/**returns first bessel function of parameter v*/
RAYX_FUNC double RAYX_API bessel1(double v);

/**
calculates the Bessel diffraction effects on circular slits and on circular
zoneplates
@params:	radius		radius < 0 (mm)
            wl			wavelength (nm)
            dphi, dpsi  angles of diffracted ray
@returns
    results stored in dphi, dpsi (inout)
*/
RAYX_FUNC void bessel_diff(double radius, double wl, double& dphi, double& dpsi, Inv& inv);

/**
 * calculates fraunhofer diffraction effects on rectangular slits
 * @param dim		dimension (x or y) (mm)
 * @param wl			wavelength (nm)
 * @param dAngle 	diffraction angle (inout)
 * @return result stored in dAngle
 */
RAYX_FUNC void fraun_diff(double dim, double wl, double& dAngle, Inv& inv);

} // namespace RAYX
