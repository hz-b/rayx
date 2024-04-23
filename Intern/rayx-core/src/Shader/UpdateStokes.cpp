#include "UpdateStokes.h"
#include "Approx.h"
#include "Rand.h"

namespace RAYX {

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

/** returns updated stokes vector
 * @param r			ray
 * @param real_S	real part of the complex s-polarization
 * @param real_P	real part of the complex p-polarization
 * @param delta		phase difference between complex s and p polarization
 * @param azimuthal	angle of rotation of element in xy plane (chi + dchi
 * (misalignment))
 * updates stokes vector of ray
 * @returns `true`, if ray should be absorbed
 */
RAYX_FUNC
bool updateStokes(Ray& r, double real_S, double real_P, double delta, double azimuthal, Inv& inv) {
    dvec4 stokes_old = r.m_stokes;
    double c_chi = r8_cos(azimuthal);
    double s_chi = r8_sin(azimuthal);
    dmat4 rot = dmat4(1, 0, 0, 0, 0, c_chi, -s_chi, 0, 0, s_chi, c_chi, 0, 0, 0, 0, 1);
    dmat4 inv_rot = transpose(rot);

    dvec4 stokes_new = mullerMatrix(real_S, real_P, delta) * rot * stokes_old;
    r.m_stokes = inv_rot * stokes_new;

    double rn = squaresDoubleRNG(inv.ctr);
    // throw ray away with certain probability
    return (r.m_stokes.x / stokes_old.x) - rn <= 0;
}

} // namespace RAYX
