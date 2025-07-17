#include "Diffraction.h"

#include <boost/math/special_functions/bessel.hpp>
#include "Approx.h"
#include "Constants.h"
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
double RAYX_API bessel1(double x) {
    // if (v < 0.0 || v > 20.0) {
    //     return 0.0;
    // }

    // double sum = 0;
    // int large = 80;

    // double PO1;
    // double PO2;
    // double FA1;
    // for (int small = 0; small <= large; small++) {
    //     PO1 = dpow(-1.0, small);
    //     PO2 = dpow(v / 2.0, 2 * small + 1);
    //     FA1 = fact(small);
    //     sum += (PO1 / (FA1 * FA1 * (small + 1))) * PO2;
    // }
    // return sum;

    if (x < 0) {
    //    x = std::abs(x);  // Nur anwendbar, wenn Jᵥ(-x) = (-1)ᵛ Jᵥ(x)
    }
    if (std::isnan(x) || std::isinf(x)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    //try {
        return boost::math::cyl_bessel_j(1.0, x);
    //}
    //catch (...) {
    //    return std::numeric_limits<double>::signaling_NaN();
    //}

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
void bessel_diff(double radius, double wl, double& __restrict dphi, double& __restrict dpsi, Rand& __restrict rand) {
    double b = glm::abs(radius) * 1e06;
    double ximax = 5.0 * wl / b;

    double rn1[3];
    double c = -1;
    while (c < 0) {  // while c = wd - rn1[2] < 0 continue
        for (int i = 0; i < 3; i++) {
            rn1[i] = rand.randomDouble();
        }

        dphi = rn1[0] * ximax;
        dpsi = rn1[1] * ximax;
        double xi = sqrt(0.5 * (dphi * dphi + dpsi * dpsi));
        double u = 2.0 * PI * b * xi / wl;
        double wd = 1;
        if (u != 0) {
            std::cout << "u: " << u << std::endl;
            wd = 2.0 * bessel1(u) / u;
            wd = wd * wd;
        }
        c = wd - rn1[2];
    }

    // 50% neg/pos sign
    dphi = glm::sign(rand.randomDouble() - 0.5) * dphi;
    dpsi = glm::sign(rand.randomDouble() - 0.5) * dpsi;
}

/**
 * calculates fraunhofer diffraction effects on rectangular slits
 * @param dim		dimension (x or y) (mm)
 * @param wl			wavelength (nm)
 * @param dAngle 	diffraction angle (inout)
 * @return result stored in dAngle
 */
RAYX_FN_ACC
void fraun_diff(double dim, double wl, double& __restrict dAngle, Rand& __restrict rand) {
    if (dim == 0) return;        // no diffraction in this direction
    double b = dim * 1e06;       // slit opening
    double div = 10.0 * wl / b;  // up to 2nd maximum

    double rna[2];  // 2 uniform random numbers in [0,1]
    double c = -1;
    while (c < 0) {  // while c = wd - uni[1] < 0 continue
        for (int i = 0; i < 2; i++) {
            rna[i] = rand.randomDouble();
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
