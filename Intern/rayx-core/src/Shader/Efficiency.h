#pragma once

#include <glm.h>

#include "Core.h"
#include "Complex.h"
#include "Constants.h"

namespace RAYX {

using Stokes = glm::dvec4;
using ElectricField = cvec3;
using LocalElectricField = cvec2;

struct FresnelCoeffs {
    double s;
    double p;
};

struct ComplexFresnelCoeffs {
    complex::Complex s;
    complex::Complex p;
};

RAYX_FN_ACC
inline double angleBetweenUnitVectors(glm::dvec3 a, glm::dvec3 b) { return glm::acos(glm::dot(a, b)); }

RAYX_FN_ACC
inline complex::Complex calcRefractAngle(const complex::Complex incidentAngle, const complex::Complex iorI, const complex::Complex iorT) {
    return complex::asin((iorI / iorT) * complex::sin(incidentAngle));
}

RAYX_FN_ACC
inline complex::Complex calcBrewstersAngle(const complex::Complex iorI, const complex::Complex iorT) { return complex::atan(iorT / iorI); }

RAYX_FN_ACC
inline complex::Complex calcCriticalAngle(const complex::Complex iorI, const complex::Complex iorT) { return complex::asin(iorT / iorI); }

RAYX_FN_ACC
inline ComplexFresnelCoeffs calcReflectAmplitude(const complex::Complex incidentAngle, const complex::Complex refractAngle,
                                                 const complex::Complex iorI, const complex::Complex iorT) {
    const auto cos_i = complex::cos(incidentAngle);
    const auto cos_t = complex::cos(refractAngle);

    const auto s = (iorI * cos_i - iorT * cos_t) / (iorI * cos_i + iorT * cos_t);
    const auto p = (iorT * cos_i - iorI * cos_t) / (iorT * cos_i + iorI * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline ComplexFresnelCoeffs calcRefractAmplitude(const complex::Complex incidentAngle, const complex::Complex refractAngle,
                                                 const complex::Complex iorI, const complex::Complex iorT) {
    const auto cos_i = complex::cos(incidentAngle);
    const auto cos_t = complex::cos(refractAngle);

    const auto s = (2.0 * iorI * cos_i) / (iorI * cos_i + iorT * cos_t);
    const auto p = (2.0 * iorI * cos_i) / (iorT * cos_i + iorI * cos_t);

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline FresnelCoeffs calcReflectIntensity(const ComplexFresnelCoeffs reflectAmplitude) {
    const auto s = (reflectAmplitude.s * complex::conj(reflectAmplitude.s)).real();
    const auto p = (reflectAmplitude.p * complex::conj(reflectAmplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline FresnelCoeffs calcRefractIntensity(const ComplexFresnelCoeffs refract_amplitude, const complex::Complex incidentAngle,
                                          const complex::Complex refractAngle, const complex::Complex iorI, const complex::Complex iorT) {
    const auto r = ((iorT * complex::cos(refractAngle)) / (iorI * complex::cos(incidentAngle))).real();

    const auto s = r * (refract_amplitude.s * complex::conj(refract_amplitude.s)).real();
    const auto p = r * (refract_amplitude.p * complex::conj(refract_amplitude.p)).real();

    return {
        .s = s,
        .p = p,
    };
}

RAYX_FN_ACC
inline cmat3 calcJonesMatrix(const ComplexFresnelCoeffs amplitude) {
    return {
        amplitude.s, 0, 0, 0, amplitude.p, 0, 0, 0, 1,
    };
}

RAYX_FN_ACC
inline cmat3 calcPolaririzationMatrix(const glm::dvec3 incidentVec, const glm::dvec3 reflectOrRefractVec, const glm::dvec3 normalVec,
                                      const ComplexFresnelCoeffs amplitude) {
    const auto s0 = glm::normalize(glm::cross(incidentVec, -normalVec));
    const auto s1 = s0;
    const auto p0 = glm::cross(incidentVec, s0);
    const auto p1 = glm::cross(reflectOrRefractVec, s0);

    const auto out = glm::dmat3(s1, p1, reflectOrRefractVec);

    const auto in = glm::dmat3(s0.x, p0.x, incidentVec.x, s0.y, p0.y, incidentVec.y, s0.z, p0.z, incidentVec.z);

    const auto jonesMatrix = calcJonesMatrix(amplitude);

    return out * jonesMatrix * in;
}

RAYX_FN_ACC
inline cmat3 calcReflectPolarizationMatrixAtNormalIncidence(const ComplexFresnelCoeffs amplitude) {
    // since no plane of incidence is defined at normal incidence,
    // s and p components are equal and only contain the base reflectivity and a phase shift of 180 degrees
    // here we apply the base reflectivity and phase shift independent of the ray direction to all components
    return {
        amplitude.s, 0, 0, 0, amplitude.s, 0, 0, 0, amplitude.s,
    };
}

RAYX_FN_ACC
inline ElectricField interceptReflect(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 reflectVec,
                                      const glm::dvec3 normalVec, const complex::Complex iorI, const complex::Complex iorT) {
    const auto incidentAngle = complex::Complex(angleBetweenUnitVectors(incidentVec, -normalVec), 0);
    const auto refractAngle = calcRefractAngle(incidentAngle, iorI, iorT);

    const auto reflectAmplitude = calcReflectAmplitude(incidentAngle, refractAngle, iorI, iorT);

    // TODO: make this more robust
    const auto isNormalIncidence = incidentVec == -normalVec;
    const auto reflectPolarizationMatrix = isNormalIncidence ? calcReflectPolarizationMatrixAtNormalIncidence(reflectAmplitude)
                                                             : calcPolaririzationMatrix(incidentVec, reflectVec, normalVec, reflectAmplitude);

    const auto reflectElectricField = reflectPolarizationMatrix * incidentElectricField;
    return reflectElectricField;
}

RAYX_FN_ACC
inline double intensity(const LocalElectricField field) {
    const auto mag = complex::abs(field);
    return glm::dot(mag, mag);
}

RAYX_FN_ACC
inline double intensity(const ElectricField field) {
    const auto mag = complex::abs(field);
    return glm::dot(mag, mag);
}

RAYX_FN_ACC
inline double intensity(const Stokes stokes) { return stokes.x; }

RAYX_FN_ACC
inline double degreeOfPolarization(const Stokes stokes) { return glm::length(glm::vec3(stokes.y, stokes.z, stokes.w)) / stokes.x; }

RAYX_FN_ACC
inline Stokes fieldToStokes(const LocalElectricField field) {
    const auto mag = complex::abs(field);
    const auto theta = complex::arg(field);

    return Stokes(mag.x * mag.x + mag.y * mag.y, mag.x * mag.x - mag.y * mag.y, 2.0 * mag.x * mag.y * glm::cos(theta.x - theta.y),
                  2.0 * mag.x * mag.y * glm::sin(theta.x - theta.y));
}

RAYX_FN_ACC
inline Stokes fieldToStokes(const ElectricField field) { return fieldToStokes(LocalElectricField(field)); }

RAYX_FN_ACC
inline LocalElectricField stokesToLocalElectricField(const Stokes stokes) {
    const auto x_real = glm::sqrt((stokes.x + stokes.y) / 2.0);

    const auto y_mag = glm::sqrt((stokes.x - stokes.y) / 2.0);
    const auto y_theta = -1.0 * glm::atan(stokes.w, stokes.z);
    const auto y = complex::polar(y_mag, y_theta);

    return LocalElectricField({x_real, 0}, y);
}

RAYX_FN_ACC
inline ElectricField stokesToElectricField(const Stokes stokes) { return ElectricField(stokesToLocalElectricField(stokes), complex::Complex(0, 0)); }

RAYX_FN_ACC
inline glm::dmat3 rotationMatrix(const glm::dvec3 forward) {
    auto up = glm::dvec3(0, 1, 0);
    glm::dvec3 right;

    if (glm::abs(glm::dot(forward, up)) < .5) {
        right = glm::normalize(glm::cross(forward, up));
        up = glm::normalize(glm::cross(right, forward));
    } else {
        right = glm::dvec3(1, 0, 0);
        up = glm::normalize(glm::cross(forward, right));
        right = glm::normalize(glm::cross(forward, up));
    }

    return glm::dmat3(right, up, forward);
}

RAYX_FN_ACC
inline glm::dmat3 rotationMatrix(const glm::dvec3 forward, const glm::dvec3 up) {
    const auto right = glm::cross(forward, up);
    return glm::dmat3(right, up, forward);
}

}  // namespace RAYX
