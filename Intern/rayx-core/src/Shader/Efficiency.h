#ifndef EFFICIENCY_H
#define EFFICIENCY_H

#include "Adapt.h"

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
dvec2 RAYX_API snell(dvec2 cos_incidence, dvec2 cn1, dvec2 cn2);

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
void RAYX_API fresnel(dvec2 cn1, dvec2 cn2, dvec2 cos_incidence, dvec2 cos_transmittance, RAYX_INOUT(dvec2) complex_S, RAYX_INOUT(dvec2) complex_P);

/** computes complex s and p polarization over all layers with fresnel and
 * snell's law
 * @param energy 				energy of ray
 * @param incidence_angle		normal incidence angle
 * @param material				material the photon collides with
 * @return complex_S			complex s-polarization
 * @return complex_P			complex p-polarization
 */
void RAYX_API reflectance(double energy, double incidence_angle, RAYX_INOUT(dvec2) complex_S, RAYX_INOUT(dvec2) complex_P, int material);
/**
 * computes complex number a + i*b in euler form:
 * euler = r * e^(i * phi) where r = sqrt(a**2 + b**2) = radius and phi =
 * atan2(a,b) = (absolute) phase
 */
dvec2 RAYX_API cartesian_to_euler(dvec2 complex);



/** computes the difference in the phases of 2 complex number written in euler
 * form: r * e^(i * phi)
 * @param euler1 		first complex number // s
 * @param euler2		second complex number // p
 * @return delta = phi1 - phi2, in [-pi/2, pi/2] degrees
 */
double phase_difference(dvec2 euler1, dvec2 euler2);

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
void efficiency(Ray r, RAYX_OUT(double) real_S, RAYX_OUT(double) real_P, RAYX_OUT(double) delta, double incidence_angle, int material);


#endif
