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
cmat3 get_polarization_matrix(
    const glm::dvec3 incident_vec,
    const glm::dvec3 reflect_or_refract_vec,
    const glm::dvec3 normal_vec,
    const ComplexCoeffs amplitude
) {
    const auto s0 = glm::normalize(glm::cross(incident_vec, -normal_vec));
    const auto s1 = s0;
    const auto p0 = glm::cross(incident_vec, s0);
    const auto p1 = glm::cross(reflect_or_refract_vec, s0);

    const auto o_out = glm::dmat3(
        s1,
        p1,
        reflect_or_refract_vec
    );

    const auto o_in = glm::dmat3(
        s0.x, p0.x, incident_vec.x,
        s0.y, p0.y, incident_vec.y,
        s0.z, p0.z, incident_vec.z
    );

    const auto jones_matrix = get_jones_matrix(amplitude);

    return o_out * jones_matrix * o_in;
}

RAYX_FUNC
inline
Field intercept_reflect(
    const Field incident_field,
    const dvec3 incident_vec,
    const dvec3 reflect_vec,
    const dvec3 normal_vec,
    const complex::Complex ior_i,
    const complex::Complex ior_t
) {
    // TODO: handle special case: dot(incident_vec, reflect_vec) == 1

    const auto incident_angle = complex::Complex(get_angle(incident_vec, -normal_vec), 0);
    const auto refract_angle = get_refract_angle(incident_angle, ior_i, ior_t);

    const auto reflect_amplitude = get_reflect_amplitude(incident_angle, refract_angle, ior_i, ior_t);

    const auto reflect_polarization_matrix = get_polarization_matrix(incident_vec, reflect_vec, normal_vec, reflect_amplitude);
    const auto reflect_field = reflect_polarization_matrix * incident_field;

    return reflect_field;
}

} // namespace RAYX
