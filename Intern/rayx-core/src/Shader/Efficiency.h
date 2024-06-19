#pragma once

#include "Common.h"
#include "InvocationState.h"
#include "RefractiveIndex.h"
#include "Complex.h"

#include <Debug/Debug.h>

namespace RAYX {

using Field = cvec3;

struct Coeffs {
    double s;
    double p;
};

struct ComplexCoeffs {
    complex::Complex s;
    complex::Complex p;
};

RAYX_FUNC
inline
double get_angle(glm::dvec3 a, glm::dvec3 b) {
    return glm::acos(glm::dot(a, b));
}

RAYX_FUNC
inline
complex::Complex get_refract_angle(const complex::Complex incident_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    return complex::asin((ior_i / ior_t) * complex::sin(incident_angle));
}

RAYX_FUNC
inline
complex::Complex get_brewsters_angle(const complex::Complex inverse_ior_ratio) {
    return complex::atan(inverse_ior_ratio);
}

RAYX_FUNC
inline
complex::Complex get_critical_angle(const complex::Complex inverse_ior_ratio) {
    return complex::asin(inverse_ior_ratio);
}

RAYX_FUNC
inline
ComplexCoeffs get_reflect_amplitude(const complex::Complex incident_angle, const complex::Complex refract_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    const auto cos_i = complex::cos(incident_angle);
    const auto cos_t = complex::cos(refract_angle);

    const auto s = (ior_i * cos_i - ior_t * cos_t) / (ior_i * cos_i + ior_t * cos_t);
    const auto p = (ior_t * cos_i - ior_i * cos_t) / (ior_t * cos_i + ior_i * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FUNC
inline
ComplexCoeffs get_refract_amplitude(const complex::Complex incident_angle, const complex::Complex refract_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    const auto cos_i = complex::cos(incident_angle);
    const auto cos_t = complex::cos(refract_angle);

    const auto s = (2.0 * ior_i * cos_i) / (ior_i * cos_i + ior_t * cos_t);
    const auto p = (2.0 * ior_i * cos_i) / (ior_t * cos_i + ior_i * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FUNC
inline
Coeffs get_reflect_intensity(const ComplexCoeffs reflect_amplitude) {
    const auto s = (reflect_amplitude.s * complex::conj(reflect_amplitude.s)).real();
    const auto p = (reflect_amplitude.p * complex::conj(reflect_amplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FUNC
inline
cmat3 get_jones_matrix(const ComplexCoeffs amplitude) {
    return {
        amplitude.s, 0, 0,
        0, amplitude.p, 0,
        0, 0, 1,
    };
}

RAYX_FUNC
inline
dmat3c get_polarization_matrix(
    const glm::dvec3 incident_vec,
    const glm::dvec3 reflect_or_refract_vec,
    const glm::dvec3 normal_vec,
    const ComplexCoeffs amplitude
) {
    const auto jones_matrix = get_jones_matrix(amplitude);

    const auto s0 = glm::normalize(glm::cross(incident_vec, -normal_vec));
    const auto s1 = s0;
    const auto p0 = glm::cross(incident_vec, s0);
    const auto p1 = glm::cross(reflect_or_refract_vec, s0);

    const auto o_out = glm::dmat3(
        s1.x, p1.x, reflect_or_refract_vec.x,
        s1.y, p1.y, reflect_or_refract_vec.y,
        s1.z, p1.z, reflect_or_refract_vec.z
    );

    const auto o_in = glm::dmat3(
        s0,
        p0,
        incident_vec
    );

    return o_out * jones_matrix * o_in;
}

RAYX_FUNC
inline
Field intercept_reflect(
    const Field incident_field,
    const double energy,
    const dvec3 incident_vec,
    const dvec3 normal_vec,
    const int material,
    Inv& inv
) {
    constexpr int vacuum_material = -1;
    const auto _ior_i = getRefractiveIndex(energy, vacuum_material, inv);
    const auto _ior_t = getRefractiveIndex(energy, material, inv);
    const auto ior_i = complex(_ior_i.x, _ior_i.y);
    const auto ior_t = complex(_ior_t.x, _ior_t.y);
    // const auto ior_i = complex(1, 0);
    // const auto ior_t = complex(1, 0);

    const auto incident_angle = complex(get_angle(incident_vec, -normal_vec), 0);
    const auto refract_angle = get_refract_angle(incident_angle, ior_i, ior_t);
    const auto reflect_vec = glm::reflect(incident_vec, normal_vec);

    const auto reflect_amplitude = get_reflect_amplitude(incident_angle, refract_angle, ior_i, ior_t);
    const auto reflect_intensity = get_reflect_intensity(reflect_amplitude);
    const auto reflect_polarization_matrix = get_polarization_matrix(incident_vec, reflect_vec, normal_vec, reflect_amplitude);
    const auto reflect_field = reflect_polarization_matrix * incident_field.e;

    return Field {
        .e = reflect_field,
        .intensity = (reflect_intensity.s + reflect_intensity.p) / 2.0,
    };
}

/**
 * computes complex number a + i*b in euler form:
 * euler = r * e^(i * phi) where r = sqrt(a**2 + b**2) = radius and phi =
 * atan2(a,b) = (absolute) phase
 */
RAYX_FUNC
inline
dvec2 RAYX_API cartesian_to_euler(dvec2 complex) {
    double r = dot(complex,
                   complex);  // r = sqrt(a**2 + b**2), why not take sqrt in fortran
                              // code?, maybe better bc square root looses precision
    dvec2 euler;
    if (r < 0 || r > 1) {
        euler = dvec2(1000000.0f, 10000000.0f);
        return euler;
    }
    euler = dvec2(r, glm::atan(complex.y, complex.x));  // phi in rad
    return euler;
}

/** computes the difference in the phases of 2 complex number written in euler
 * form: r * e^(i * phi)
 * @param euler1 		first complex number // s
 * @param euler2		second complex number // p
 * @return delta = phi1 - phi2, in [-pi/2, pi/2] degrees
 */
RAYX_FUNC
inline
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
RAYX_FUNC
inline
void efficiency(Ray r, double& real_S, double& real_P, double& delta, double incident_angle, int material, Inv& inv) {
    constexpr int vacuum_material = -1;
    const auto _ior_i = getRefractiveIndex(r.m_energy, vacuum_material, inv);
    const auto _ior_t = getRefractiveIndex(r.m_energy, material, inv);
    const auto ior_i = complex(_ior_i.x, _ior_i.y);
    const auto ior_t = complex(_ior_t.x, _ior_t.y);

    const auto refract_angle = get_refract_angle(incident_angle, ior_i, ior_t);

    const auto reflect_amplitude = get_reflect_amplitude(incident_angle, refract_angle, ior_i, ior_t);
    const auto reflect_intensity = get_reflect_intensity(reflect_amplitude);

    dvec2 euler_P = cartesian_to_euler(glm::dvec2(reflect_amplitude.s.real(), reflect_amplitude.s.imag()));
    dvec2 euler_S = cartesian_to_euler(glm::dvec2(reflect_amplitude.p.real(), reflect_amplitude.p.imag()));

    delta = phase_difference(euler_S, euler_P);
    real_S = euler_S.x;
    real_P = euler_P.x;
}


} // namespace RAYX
