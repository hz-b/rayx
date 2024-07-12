#include "Diffraction.h"

#include "Approx.h"
#include "Constants.h"
#include "InvocationState.h"
#include "Rand.h"

namespace RAYX {

RAYX_FN_ACC
double RAYX_API fact(int a) {
    if (a < 0) {
        return a;
    }
    double f = 1;
    for (int i = 2; i <= a; i++) {
        f *= i;
    }
    return f;
}

/**returns first bessel function of parameter v*/
RAYX_FN_ACC
double RAYX_API bessel1(double v) {
    if (v < 0.0 || v > 20.0) {
        return 0.0;
    }

    double sum = 0;
    int large = 30;

    double PO1;
    double PO2;
    double FA1;
    for (int small = 0; small <= large; small++) {
        PO1 = dpow(-1.0, small);
        PO2 = dpow(v / 2.0, 2 * small + 1);
        FA1 = fact(small);
        sum += (PO1 / (FA1 * FA1 * (small + 1))) * PO2;
    }
    return sum;
}

/**
calculates the Bessel diffraction effects on circular slits and on circular
zoneplates
@params:	radius		radius < 0 (mm)
            wl			wavelength (nm)
            dphi, dpsi  angles of diffracted ray
@returns
    results stored in dphi, dpsi (inout)
*/
RAYX_FN_ACC
void bessel_diff(double radius, double wl, double& dphi, double& dpsi, InvState& inv) {
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
RAYX_FN_ACC
void fraun_diff(double dim, double wl, double& dAngle, InvState& inv) {
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
        double u = PI * b * glm::sin(dAngle) / wl;
        double wd = 1;
        if (u != 0) {
            wd = glm::sin(u) / u;
            wd = wd * wd;
        }
        c = wd - rna[1];
    }
}

}  // namespace RAYX
