#include "Efficiency.h"
#include "Complex.h"
#include "Utils.h"

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
RAYX_FUNC
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
RAYX_FUNC
void RAYX_API fresnel(dvec2 cn1, dvec2 cn2, dvec2 cos_incidence, dvec2 cos_transmittance, dvec2& complex_S, dvec2& complex_P) {
    dvec2 mat1_incidence = cx_mul(cn1, cos_incidence);  // material one (incoming side)
    dvec2 mat1_transmittance = cx_mul(cn1, cos_transmittance);
    dvec2 mat2_incidence = cx_mul(cn2, cos_incidence);  // material two (into which the ray goes)
    dvec2 mat2_transmittance = cx_mul(cn2, cos_transmittance);

    complex_S = cx_div(cx_sub(mat1_incidence, mat2_transmittance), cx_add(mat1_incidence, mat2_transmittance));
    complex_P = -cx_div(cx_sub(mat1_transmittance, mat2_incidence), cx_add(mat1_transmittance, mat2_incidence));
    return;
}

/**
 * computes complex number a + i*b in euler form:
 * euler = r * e^(i * phi) where r = sqrt(a**2 + b**2) = radius and phi =
 * atan2(a,b) = (absolute) phase
 */
RAYX_FUNC
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
RAYX_FUNC
double phase_difference(dvec2 euler1, dvec2 euler2) {
    double delta = euler2.y - euler1.y;  // p - s
    delta = delta - int(delta > 180) * 360.0 + int(delta < -180) * 360.0;
    return delta;
}
