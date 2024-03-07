#ifndef DIFFRACTION_H
#define DIFFRACTION_H

#include "Common.h"
#include "InvocationState.h"
#include "Approx.h"
#include "Rand.h"

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
template <typename MemSpace>
RAYX_FUNC_INLINE
void bessel_diff(double radius, double wl, double& dphi, double& dpsi, Inv<MemSpace>& inv) {
    double b = abs(radius) * 1e06;
    double ximax = 5.0 * wl / b;

    double rn1[3];
    double c = -1;
    while (c < 0) {  // while c = wd - rn1[2] < 0 continue
        for (int i = 0; i < 3; i++) {
            rn1[i] = squaresDoubleRNG(inv.ctr);
        }

        dphi = rn1[0] * ximax;
        dpsi = rn1[1] * ximax;
        double xi = sqrt(0.5 * (dphi * dphi + dpsi * dpsi));
        double u = 2.0 * PI * b * xi / wl;
        double wd = 1;
        if (u != 0) {
            wd = 2.0 * bessel1(u) / u;
            wd = wd * wd;
        }
        c = wd - rn1[2];
    }

    // 50% neg/pos sign
    dphi = sign(squaresDoubleRNG(inv.ctr) - 0.5) * dphi;
    dpsi = sign(squaresDoubleRNG(inv.ctr) - 0.5) * dpsi;
}

/**
 * calculates fraunhofer diffraction effects on rectangular slits
 * @param dim		dimension (x or y) (mm)
 * @param wl			wavelength (nm)
 * @param dAngle 	diffraction angle (inout)
 * @return result stored in dAngle
 */
template <typename MemSpace>
RAYX_FUNC_INLINE
void fraun_diff(double dim, double wl, double& dAngle, Inv<MemSpace>& inv) {
    if (dim == 0) return;        // no diffraction in this direction
    double b = dim * 1e06;       // slit opening
    double div = 10.0 * wl / b;  // up to 2nd maximum

    double rna[2];  // 2 uniform random numbers in [0,1]
    double c = -1;
    while (c < 0) {  // while c = wd - uni[1] < 0 continue
        for (int i = 0; i < 2; i++) {
            rna[i] = squaresDoubleRNG(inv.ctr);
        }
        dAngle = (rna[0] - 0.5) * div;
        double u = PI * b * r8_sin(dAngle) / wl;
        double wd = 1;
        if (u != 0) {
            wd = r8_sin(u) / u;
            wd = wd * wd;
        }
        c = wd - rna[1];
    }
}

#endif
