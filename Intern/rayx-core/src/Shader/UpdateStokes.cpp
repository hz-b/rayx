#include "UpdateStokes.h"

/** creates a Müller matrix (see p. 32 of Schäfers, 2007)
 * @param R_s
 * @param R_p
 * @param delta
 *
 */
RAYX_FUNC
dmat4 mullerMatrix(double R_s, double R_p, double delta) {
    double c = r8_cos(delta);
    double s = r8_sin(delta);
    return dmat4((R_s + R_p) / 2.0, (R_p - R_s) / 2.0, 0.0, 0.0, (R_p - R_s) / 2.0, (R_s + R_p) / 2.0, 0.0, 0.0, 0.0, 0.0, R_s * R_p * c,
                 R_s * R_p * s, 0.0, 0.0, -R_s * R_p * s, R_s * R_p * c);
}
