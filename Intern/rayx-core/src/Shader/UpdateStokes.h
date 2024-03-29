#ifndef UPDATE_STOKES_H
#define UPDATE_STOKES_H

#include "Adapt.h"

/** creates a Müller matrix (see p. 32 of Schäfers, 2007)
 * @param R_s
 * @param R_p
 * @param delta
 *
 */
dmat4 mullerMatrix(double R_s, double R_p, double delta);

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
bool updateStokes(RAYX_INOUT(Ray) r, double real_S, double real_P, double delta, double azimuthal);

#endif
