#pragma once

#include "Common.h"
#include "Complex.h"
#include "Constants.h"

namespace RAYX {

using Stokes = dvec4;
using Field = cvec3;
using LocalField = cvec2;

struct FresnelCoeffs {
    double s;
    double p;
};

struct ComplexFresnelCoeffs {
    complex::Complex s;
    complex::Complex p;
};

RAYX_FN_ACC
inline double angleBetweenUnitVectors(glm::dvec3 a, glm::dvec3 b) {
    return glm::acos(glm::dot(a, b));
}

RAYX_FN_ACC
inline complex::Complex refractAngle(const complex::Complex incident_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    return complex::asin((ior_i / ior_t) * complex::sin(incident_angle));
}

RAYX_FN_ACC
inline complex::Complex brewstersAngle(const complex::Complex ior_i, const complex::Complex ior_t) {
    return complex::atan(ior_t / ior_i);
}

RAYX_FN_ACC
inline complex::Complex criticalAngle(const complex::Complex ior_i, const complex::Complex ior_t) {
    return complex::asin(ior_t / ior_i);
}

RAYX_FN_ACC
inline ComplexFresnelCoeffs reflectAmplitude(const complex::Complex incident_angle, const complex::Complex refract_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    const auto cos_i = complex::cos(incident_angle);
    const auto cos_t = complex::cos(refract_angle);

    const auto s = (ior_i * cos_i - ior_t * cos_t) / (ior_i * cos_i + ior_t * cos_t);
    const auto p = (ior_t * cos_i - ior_i * cos_t) / (ior_t * cos_i + ior_i * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline ComplexFresnelCoeffs refractAmplitude(const complex::Complex incident_angle, const complex::Complex refract_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    const auto cos_i = complex::cos(incident_angle);
    const auto cos_t = complex::cos(refract_angle);

    const auto s = (2.0 * ior_i * cos_i) / (ior_i * cos_i + ior_t * cos_t);
    const auto p = (2.0 * ior_i * cos_i) / (ior_t * cos_i + ior_i * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline FresnelCoeffs reflectIntensity(const ComplexFresnelCoeffs reflect_amplitude) {
    const auto s = (reflect_amplitude.s * complex::conj(reflect_amplitude.s)).real();
    const auto p = (reflect_amplitude.p * complex::conj(reflect_amplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline FresnelCoeffs refractIntensity(const ComplexFresnelCoeffs refract_amplitude, const complex::Complex incident_angle, const complex::Complex refract_angle, const complex::Complex ior_i, const complex::Complex ior_t) {
    const auto r = ((ior_t * complex::cos(refract_angle)) / (ior_i * complex::cos(incident_angle))).real();

    const auto s = r * (refract_amplitude.s * complex::conj(refract_amplitude.s)).real();
    const auto p = r * (refract_amplitude.p * complex::conj(refract_amplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline cmat3 jonesMatrix(const ComplexFresnelCoeffs amplitude) {
    return {
        amplitude.s, 0, 0,
        0, amplitude.p, 0,
        0, 0, 1,
    };
}

RAYX_FN_ACC
inline cmat3 polarizationMatrix(
    const glm::dvec3 incident_vec,
    const glm::dvec3 reflect_or_refract_vec,
    const glm::dvec3 normal_vec,
    const ComplexFresnelCoeffs amplitude
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

    const auto jones_matrix = jonesMatrix(amplitude);

    return o_out * jones_matrix * o_in;
}

RAYX_FN_ACC
inline cmat3 reflectPolarizationMatrixAtNormalIncidence(const ComplexFresnelCoeffs amplitude) {
    // since no plane of incidence is defined at normal incidence,
    // s and p components are equal and only contain the base reflectivity and a phase shift of 180 degrees
    // here we apply the base reflectivity and phase shift independent of the ray direction to all components
    return {
        amplitude.s, 0, 0,
        0, amplitude.s, 0,
        0, 0, amplitude.s,
    };
}

RAYX_FN_ACC
inline Field interceptReflect(
    const Field incident_field,
    const dvec3 incident_vec,
    const dvec3 reflect_vec,
    const dvec3 normal_vec,
    const complex::Complex ior_i,
    const complex::Complex ior_t
) {
    const auto incident_angle = complex::Complex(angleBetweenUnitVectors(incident_vec, -normal_vec), 0);
    const auto refract_angle = refractAngle(incident_angle, ior_i, ior_t);

    const auto reflect_amplitude = reflectAmplitude(incident_angle, refract_angle, ior_i, ior_t);

    // TODO: make this more robust
    const auto is_normal_incidence = incident_vec == -normal_vec;
    const auto reflect_polarization_matrix =
        is_normal_incidence
        ? reflectPolarizationMatrixAtNormalIncidence(reflect_amplitude)
        : polarizationMatrix(incident_vec, reflect_vec, normal_vec, reflect_amplitude);

    const auto reflect_field = reflect_polarization_matrix * incident_field;
    return reflect_field;
}

RAYX_FN_ACC
inline double intensity(const LocalField field) {
    const auto mag = complex::abs(field);
    return glm::dot(mag, mag);
}

RAYX_FN_ACC
inline double intensity(const Field field) {
    const auto mag = complex::abs(field);
    return glm::dot(mag, mag);
}

RAYX_FN_ACC
inline double intensity(const Stokes stokes) {
    return stokes.x;
}

RAYX_FN_ACC
inline double degreeOfPolarization(const Stokes stokes) {
    return glm::length(glm::vec3(stokes.y, stokes.z, stokes.w)) / stokes.x;
}

RAYX_FN_ACC
inline Stokes fieldToStokes(const LocalField field) {
    const auto mag = complex::abs(field);
    const auto theta = complex::arg(field);

    return Stokes(
        mag.x*mag.x + mag.y*mag.y,
        mag.x*mag.x - mag.y*mag.y,
        2.0 * mag.x * mag.y * glm::cos(theta.x - theta.y),
        2.0 * mag.x * mag.y * glm::sin(theta.x - theta.y)
    );
}

RAYX_FN_ACC
inline Stokes fieldToStokes(const Field field) {
    return fieldToStokes(LocalField(field));
}

RAYX_FN_ACC
inline LocalField stokesToLocalField(const Stokes stokes) {
    const auto x_real = glm::sqrt((stokes.x + stokes.y) / 2.0);

    const auto y_mag = glm::sqrt((stokes.x - stokes.y) / 2.0);
    const auto y_theta = -1.0 * glm::atan(stokes.w, stokes.z);
    const auto y = complex::polar(y_mag, y_theta);

    return LocalField(
        {x_real, 0},
        y
    );
}

RAYX_FN_ACC
inline Field stokesToField(const Stokes stokes) {
    return Field(stokesToLocalField(stokes), complex::Complex(0, 0));
}

RAYX_FN_ACC
inline dmat3 rotationMatrix(const dvec3 forward) {
    auto up = dvec3(0, 1, 0);
    dvec3 right;

    if (glm::abs(glm::dot(forward, up)) < .5) {
        right = glm::normalize(glm::cross(forward, up));
        up = glm::normalize(glm::cross(right, forward));
    } else {
        right = dvec3(1, 0, 0);
        up = glm::normalize(glm::cross(forward, right));
        right = glm::normalize(glm::cross(forward, up));
    }

    return dmat3(right, up, forward);
}

RAYX_FN_ACC
inline dmat3 rotationMatrix(const dvec3 forward, const dvec3 up) {
    const auto right = glm::cross(forward, up);
    return dmat3(right, up, forward);
}

} // namespace RAYX
