#pragma once

#include "ElectricField.h"

namespace RAYX {

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
    // TODO: cross product is not numerically stable here, if indicentVec == reflectOrRefractVec
    // we dont need the lenght of the vector resulting from the cross product, but only the direciton.
    // this may could be done by calculation angles instead of a vector.
    // fixing this removes the check in interceptReflect, that specializes for normal incidence
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

}  // namespace RAYX
