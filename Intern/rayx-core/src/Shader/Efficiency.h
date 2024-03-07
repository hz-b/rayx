#ifndef EFFICIENCY_H
#define EFFICIENCY_H

#include "Common.h"
#include "InvocationState.h"
#include "Approx.h"
#include "RefractiveIndex.h"

/** calculates cosinus of transmittance angle with snell's law
 * cosinus is needed in fresnel formula
 * sin(incidence_angle)² = 1 - cos(incidence_angle)²
 * ratio² = (cn1 / cn2)²
 * sin(transmittance_angle)² = (cn1 / cn2)² * sin(incidence_angle)²
 * cos(transmittance_angle) = sqrt( 1 - sin(transmittance_angle)²)
 *
 * @param cos_incidence			cosinus of complex incidence angle
 * @param cn1					complex refractive index of material from which
 * the ray is coming
 * @param cn2					complex refractive index of material into which
 * the ray is going
 * @return cos_transmittance 	cosinus of complex transmittance angle
 */
RAYX_FUNC dvec2 RAYX_API snell(dvec2 cos_incidence, dvec2 cn1, dvec2 cn2);

/** calculates complex s- and p-polarization with fresnel formulas
 * @param cn1					complex refractive index of material from which
 * the ray is coming
 * @param cn2					complex refractive index of material into which
 * the ray is going
 * @param cos_incidence			cosinus of complex incidence angle
 * @param cos_transmittance 	cosinus of complex transmittance angle
 * @return complex_S			complex s-polarization
 * @return complex_P			complex p-polarization
 *
 */
RAYX_FUNC void RAYX_API fresnel(dvec2 cn1, dvec2 cn2, dvec2 cos_incidence, dvec2 cos_transmittance, dvec2& complex_S, dvec2& complex_P);

/** computes complex s and p polarization over all layers with fresnel and
 * snell's law
 * @param energy 				energy of ray
 * @param incidence_angle		normal incidence angle
 * @param material				material the photon collides with
 * @return complex_S			complex s-polarization
 * @return complex_P			complex p-polarization
 */
template <typename MemSpace>
RAYX_FUNC_INLINE
void RAYX_API reflectance(double energy, double incidence_angle, dvec2& complex_S, dvec2& complex_P, int material, Inv<MemSpace>& inv) {
    dvec2 cos_incidence = dvec2(r8_cos(incidence_angle),
                                0.0);  // complex number, initialization only for first layer, the
                                       // others are then derived from this with snell's law

    const int vacuum_material = -1;

    // todo number of layers: loop over layers from top to bottom, transmittance
    // angle for each layer, so far only one layer (substrate?) store cosinuses
    // in array, bc needed in later loop for fresnel (or maybe only one loop is
    // enough?) todo refractive indices of materials in extra buffer?
    dvec2 cn1 = getRefractiveIndex(energy, vacuum_material, inv);
    dvec2 cn2 = getRefractiveIndex(energy, material, inv);
    dvec2 cos_transmittance = snell(cos_incidence, cn1, cn2);

    // todo again iterate over layers but from bottom to top, update s and p
    // polarization in each iteration:
    fresnel(cn2, cn1, cos_transmittance, cos_incidence, complex_S, complex_P);
    // todo roughness (uses wavelength/energy)
    // todo other calculations for layers that are not substrate (which is the
    // first layer)
    return;
}

/**
 * computes complex number a + i*b in euler form:
 * euler = r * e^(i * phi) where r = sqrt(a**2 + b**2) = radius and phi =
 * atan2(a,b) = (absolute) phase
 */
RAYX_FUNC dvec2 RAYX_API cartesian_to_euler(dvec2 complex);

/** computes the difference in the phases of 2 complex number written in euler
 * form: r * e^(i * phi)
 * @param euler1 		first complex number // s
 * @param euler2		second complex number // p
 * @return delta = phi1 - phi2, in [-pi/2, pi/2] degrees
 */
RAYX_FUNC double phase_difference(dvec2 euler1, dvec2 euler2);

/** efficiency calculation
 * uses complex numbers for s- and p-polarisation:
 * complex_S = a + i*b as dvec2.
 * in euler form: complex_S = r * e^(i * phi) where r = sqrt(a**2 + b**2) =
 * radius and phi = atan2(a,b) = (absolute) phase
 * @param real_S				inout, s-polarization (senkrecht), radius of
 * complex number
 * @param real_P				inout, p-polarization (parallel), radius of
 * complex number
 * @param delta					phase difference between angles of complex
 * numbers complex_S, complex_P
 * @param incidence_angle		normal incidence angle
 * @param material				material the photon collides with
 * @param others
 */
template <typename MemSpace>
RAYX_FUNC_INLINE
void efficiency(Ray r, double& real_S, double& real_P, double& delta, double incidence_angle, int material, Inv<MemSpace>& inv) {
    dvec2 complex_S, complex_P;
    reflectance(r.m_energy, incidence_angle, complex_S, complex_P, material, inv);

    dvec2 euler_P = cartesian_to_euler(complex_P);
    dvec2 euler_S = cartesian_to_euler(complex_S);

    delta = phase_difference(euler_S, euler_P);
    real_S = euler_S.x;
    real_P = euler_P.x;
}

#endif
