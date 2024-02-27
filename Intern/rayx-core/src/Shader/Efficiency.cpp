#include "Efficiency.h"
#include "Approx.h"
#include "Complex.h"
#include "Utils.h"
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
dvec2 RAYX_API snell(dvec2 cos_incidence, dvec2 cn1, dvec2 cn2) {
    /*dvec2 sin_incidence = cx_sqrt( cx_sub(dvec2(1.0,0.0),
    cx_mul(cos_incidence, cos_incidence)) ); dvec2 refractive_ratio =
    cx_div(cn1, cn2); dvec2 sin_transmittance = cx_mul(sin_incidence,
    refractive_ratio); dvec2 cos_transmittance = cx_sqrt( cx_sub(dvec2(1.0,0.0),
    cx_mul(sin_transmittance, sin_transmittance))); return cos_transmittance;*/
    dvec2 sin_incidence_square = cx_sub(dvec2(1.0, 0.0), cx_mul(cos_incidence, cos_incidence));  // 1 - cos(inc)**2
    dvec2 refractive_ratio = cx_div(cn1, cn2);                                                   // cn1 / cn2
    dvec2 ratio_square = cx_mul(refractive_ratio, refractive_ratio);
    dvec2 sin_transmittance_square = cx_mul(sin_incidence_square,
                                            ratio_square);  // sin(tr)**2 = sin(inc)**2 * (cn1/cn2)**2
    dvec2 cos_transmittance = cx_sqrt(cx_sub(dvec2(1.0, 0.0),
                                             sin_transmittance_square));  // cos = sqrt( 1 - ratio**2*sin(tr)**2 )
    return cos_transmittance;
}

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
void RAYX_API fresnel(dvec2 cn1, dvec2 cn2, dvec2 cos_incidence, dvec2 cos_transmittance, RAYX_INOUT(dvec2) complex_S, RAYX_INOUT(dvec2) complex_P) {
    dvec2 mat1_incidence = cx_mul(cn1, cos_incidence);  // material one (incoming side)
    dvec2 mat1_transmittance = cx_mul(cn1, cos_transmittance);
    dvec2 mat2_incidence = cx_mul(cn2, cos_incidence);  // material two (into which the ray goes)
    dvec2 mat2_transmittance = cx_mul(cn2, cos_transmittance);

    complex_S = cx_div(cx_sub(mat1_incidence, mat2_transmittance), cx_add(mat1_incidence, mat2_transmittance));
    complex_P = -cx_div(cx_sub(mat1_transmittance, mat2_incidence), cx_add(mat1_transmittance, mat2_incidence));
    return;
}

/** computes complex s and p polarization over all layers with fresnel and
 * snell's law
 * @param energy 				energy of ray
 * @param incidence_angle		normal incidence angle
 * @param material				material the photon collides with
 * @return complex_S			complex s-polarization
 * @return complex_P			complex p-polarization
 */
void RAYX_API reflectance(double energy, double incidence_angle, RAYX_INOUT(dvec2) complex_S, RAYX_INOUT(dvec2) complex_P, int material, Inv& inv) {
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
dvec2 RAYX_API cartesian_to_euler(dvec2 complex) {
    double r = dot(complex,
                   complex);  // r = sqrt(a**2 + b**2), why not take sqrt in fortran
                              // code?, maybe better bc square root looses precision
    dvec2 euler;
    if (r < 0 || r > 1) {
        euler = dvec2(infinity(), infinity());
        return euler;
    }
    euler = dvec2(r, r8_atan2(complex.y, complex.x));  // phi in rad
    return euler;
}

/** computes the difference in the phases of 2 complex number written in euler
 * form: r * e^(i * phi)
 * @param euler1 		first complex number // s
 * @param euler2		second complex number // p
 * @return delta = phi1 - phi2, in [-pi/2, pi/2] degrees
 */
double phase_difference(dvec2 euler1, dvec2 euler2) {
    double delta = euler2.y - euler1.y;  // p - s
    delta = delta - int(delta > 180) * 360.0 + int(delta < -180) * 360.0;
    return delta;
}

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
void efficiency(Ray r, RAYX_OUT(double) real_S, RAYX_OUT(double) real_P, RAYX_OUT(double) delta, double incidence_angle, int material, Inv& inv) {
    dvec2 complex_S, complex_P;
    reflectance(r.m_energy, incidence_angle, complex_S, complex_P, material, inv);

    dvec2 euler_P = cartesian_to_euler(complex_P);
    dvec2 euler_S = cartesian_to_euler(complex_S);

    delta = phase_difference(euler_S, euler_P);
    real_S = euler_S.x;
    real_P = euler_P.x;
}

