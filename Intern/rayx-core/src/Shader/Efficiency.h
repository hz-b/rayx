#pragma once

#include <glm.h>

#include "Complex.h"
#include "Constants.h"
#include "Core.h"

namespace RAYX {

using Stokes = glm::dvec4;         // Stokes vector represents the polarization state of light in the ray local plane.
using LocalElectricField = cvec2;  // Electric field in the ray local plane, as a complex 2D vector.
using ElectricField = cvec3;       // Electric field as a complex 3D vector.

struct FresnelCoeffs {
    double s;  // Real component of Fresnel reflection/refraction coefficient for s-polarized light.
    double p;  // Real component of Fresnel reflection/refraction coefficient for p-polarized light.
};

struct ComplexFresnelCoeffs {
    complex::Complex s;  // Complex Fresnel coefficient for s-polarized light.
    complex::Complex p;  // Complex Fresnel coefficient for p-polarized light.
};

// Computes the angle between two unit vectors in 3D space.
RAYX_FN_ACC
inline double angleBetweenUnitVectors(glm::dvec3 a, glm::dvec3 b) { return glm::acos(glm::dot(a, b)); }

// Calculates the refracted angle using Snell's Law, based on complex index of refraction.
RAYX_FN_ACC
inline complex::Complex calcRefractAngle(const complex::Complex incidentAngle, const complex::Complex iorI, const complex::Complex iorT) {
    return complex::asin((iorI / iorT) * complex::sin(incidentAngle));
}

// Calculates Brewster's angle, the angle at which no reflection occurs for p-polarized light.
RAYX_FN_ACC
inline complex::Complex calcBrewstersAngle(const complex::Complex iorI, const complex::Complex iorT) { return complex::atan(iorT / iorI); }

// Calculates the critical angle for total internal reflection.
RAYX_FN_ACC
inline complex::Complex calcCriticalAngle(const complex::Complex iorI, const complex::Complex iorT) { return complex::asin(iorT / iorI); }

// Computes the Fresnel reflection amplitude for s and p polarization states.
RAYX_FN_ACC
inline ComplexFresnelCoeffs calcReflectAmplitude(const complex::Complex incidentAngle, const complex::Complex refractAngle,
                                                 const complex::Complex iorI, const complex::Complex iorT) {
    const auto cos_i = complex::cos(incidentAngle);
    const auto cos_t = complex::cos(refractAngle);

    const auto s = (iorI * cos_i - iorT * cos_t) / (iorI * cos_i + iorT * cos_t);
    const auto p = (iorT * cos_i - iorI * cos_t) / (iorT * cos_i + iorI * cos_t);

    return {.s = s, .p = p};
}

// Computes the Fresnel transmission (refracted) amplitude for s and p polarization states.
RAYX_FN_ACC
inline ComplexFresnelCoeffs calcRefractAmplitude(const complex::Complex incidentAngle, const complex::Complex refractAngle,
                                                 const complex::Complex iorI, const complex::Complex iorT) {
    const auto cos_i = complex::cos(incidentAngle);
    const auto cos_t = complex::cos(refractAngle);

    const auto s = (2.0 * iorI * cos_i) / (iorI * cos_i + iorT * cos_t);
    const auto p = (2.0 * iorI * cos_i) / (iorT * cos_i + iorI * cos_t);

    return {.s = s, .p = p};
}

// Computes the reflectance (intensity) based on the Fresnel reflection amplitudes.
RAYX_FN_ACC
inline FresnelCoeffs calcReflectIntensity(const ComplexFresnelCoeffs reflectAmplitude) {
    const auto s = (reflectAmplitude.s * complex::conj(reflectAmplitude.s)).real();  // s-polarized intensity.
    const auto p = (reflectAmplitude.p * complex::conj(reflectAmplitude.p)).real();  // p-polarized intensity.

    return {.s = s, .p = p};
}

// Computes the transmittance (intensity) based on the Fresnel transmission amplitudes.
RAYX_FN_ACC
inline FresnelCoeffs calcRefractIntensity(const ComplexFresnelCoeffs refract_amplitude, const complex::Complex incidentAngle,
                                          const complex::Complex refractAngle, const complex::Complex iorI, const complex::Complex iorT) {
    const auto r = ((iorT * complex::cos(refractAngle)) / (iorI * complex::cos(incidentAngle))).real();

    const auto s = r * (refract_amplitude.s * complex::conj(refract_amplitude.s)).real();  // s-polarized intensity.
    const auto p = r * (refract_amplitude.p * complex::conj(refract_amplitude.p)).real();  // p-polarized intensity.

    return {.s = s, .p = p};
}

// Computes the Jones matrix, representing the effect on the polarization state, based on Fresnel coefficients.
RAYX_FN_ACC
inline cmat3 calcJonesMatrix(const ComplexFresnelCoeffs amplitude) { return {amplitude.s, 0, 0, 0, amplitude.p, 0, 0, 0, 1}; }

// Computes the polarization transformation matrix for reflection/refraction.
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

// Computes the polarization matrix for reflection at normal incidence (simplified case).
RAYX_FN_ACC
inline cmat3 calcReflectPolarizationMatrixAtNormalIncidence(const ComplexFresnelCoeffs amplitude) {
    return {
        amplitude.s, 0, 0, 0, amplitude.s, 0, 0, 0, amplitude.s,  // All components equal since it's normal incidence.
    };
}

// Calculates the reflected electric field after intercepting a surface.
RAYX_FN_ACC
inline ElectricField interceptReflect(const ElectricField incidentElectricField, const glm::dvec3 incidentVec, const glm::dvec3 reflectVec,
                                      const glm::dvec3 normalVec, const complex::Complex iorI, const complex::Complex iorT) {
    const auto incidentAngle = complex::Complex(angleBetweenUnitVectors(incidentVec, -normalVec), 0);
    const auto refractAngle = calcRefractAngle(incidentAngle, iorI, iorT);

    const auto reflectAmplitude = calcReflectAmplitude(incidentAngle, refractAngle, iorI, iorT);

    const auto isNormalIncidence = incidentVec == -normalVec;  // Check for normal incidence.
    const auto reflectPolarizationMatrix = isNormalIncidence ? calcReflectPolarizationMatrixAtNormalIncidence(reflectAmplitude)
                                                             : calcPolaririzationMatrix(incidentVec, reflectVec, normalVec, reflectAmplitude);

    const auto reflectElectricField = reflectPolarizationMatrix * incidentElectricField;  // Reflect electric field.
    return reflectElectricField;
}

// Computes the intensity from a 2D electric field (local).
RAYX_FN_ACC
inline double intensity(const LocalElectricField field) {
    const auto mag = complex::abs(field);
    return glm::dot(mag, mag);
}

// Computes the intensity from a 3D electric field (global).
RAYX_FN_ACC
inline double intensity(const ElectricField field) {
    const auto mag = complex::abs(field);
    return glm::dot(mag, mag);
}

// Returns the intensity from a Stokes vector.
RAYX_FN_ACC
inline double intensity(const Stokes stokes) { return stokes.x; }

// Computes the degree of polarization from a Stokes vector.
RAYX_FN_ACC
inline double degreeOfPolarization(const Stokes stokes) { return glm::length(glm::vec3(stokes.y, stokes.z, stokes.w)) / stokes.x; }

// Converts an electric field to a Stokes vector.
RAYX_FN_ACC
inline Stokes localElectricFieldToStokes(const LocalElectricField field) {
    const auto mag = complex::abs(field);
    const auto theta = complex::arg(field);

    return Stokes(mag.x * mag.x + mag.y * mag.y, mag.x * mag.x - mag.y * mag.y, 2.0 * mag.x * mag.y * glm::cos(theta.x - theta.y),
                  2.0 * mag.x * mag.y * glm::sin(theta.x - theta.y));
}

// Converts a Stokes vector back into a local electric field.
RAYX_FN_ACC
inline LocalElectricField stokesToLocalElectricField(const Stokes stokes) {
    const auto x_real = glm::sqrt((stokes.x + stokes.y) / 2.0);

    const auto y_mag = glm::sqrt((stokes.x - stokes.y) / 2.0);
    const auto y_theta = -1.0 * glm::atan(stokes.w, stokes.z);
    const auto y = complex::polar(y_mag, y_theta);

    return LocalElectricField({x_real, 0}, y);
}

struct RotationBase {
    glm::dvec3 right;
    glm::dvec3 up;
    glm::dvec3 forward;
};

// Computes a base given a forward vector.
// A convention for the up and right vectors is implemented, making this function well defined and for all forward directions.
// TODO(Sven): this convention should be exchanged with one that does not branch
RAYX_FN_ACC
inline RotationBase forwardVectorToBaseConvention(glm::dvec3 forward) {
    auto up = glm::dvec3(0, 1, 0);
    glm::dvec3 right;

    // If the forward vector is not nearly vertical, we use the up vector (0, 1, 0), othwewise use the right vector (1, 0, 0)
    if (glm::abs(glm::dot(forward, up)) < .5) {
        right = glm::normalize(glm::cross(up, forward));
        up = glm::cross(forward, right);
    } else {
        right = glm::dvec3(1, 0, 0);
        up = glm::normalize(glm::cross(forward, right));
        right = glm::cross(up, forward);
    }

    return RotationBase{
        .right = right,
        .up = up,
        .forward = forward,
    };
}

// Computes a rotation matrix given a forward vector. This matrix can be used to align an object with a direction.
RAYX_FN_ACC
inline glm::dmat3 rotationMatrix(const glm::dvec3 forward) {
    const auto base = forwardVectorToBaseConvention(forward);
    return glm::dmat3(base.right, base.up, base.forward);
}

// Computes a rotation matrix given both forward and up vectors, used to align objects in space.
RAYX_FN_ACC
inline glm::dmat3 rotationMatrix(const glm::dvec3 forward, const glm::dvec3 up) {
    const auto right = glm::cross(forward, up);
    return glm::dmat3(right, up, forward);
}

RAYX_FN_ACC
inline ElectricField localToGlobalElectricField(LocalElectricField localField, glm::dvec3 forward) {
    return rotationMatrix(forward) * ElectricField(localField, complex::Complex{0, 0});
}

RAYX_FN_ACC
inline ElectricField localToGlobalElectricField(LocalElectricField localField, glm::dvec3 forward, glm::dvec3 up) {
    return rotationMatrix(forward, up) * ElectricField(localField, complex::Complex{0, 0});
}

RAYX_FN_ACC
inline LocalElectricField globalToLocalElectricField(ElectricField field, glm::dvec3 forward) {
    return glm::transpose(rotationMatrix(forward)) * field;
}

RAYX_FN_ACC
inline LocalElectricField globalToLocalElectricField(ElectricField field, glm::dvec3 forward, glm::vec3 up) {
    return glm::transpose(rotationMatrix(forward, up)) * field;
}

RAYX_FN_ACC
inline ElectricField stokesToElectricField(const Stokes stokes, const glm::dvec3 forward) {
    return localToGlobalElectricField(stokesToLocalElectricField(stokes), forward);
}

RAYX_FN_ACC
inline ElectricField stokesToElectricField(const Stokes stokes, const glm::dvec3 forward, const glm::dvec3 up) {
    return localToGlobalElectricField(stokesToLocalElectricField(stokes), forward, up);
}

RAYX_FN_ACC
inline ElectricField stokesToElectricField(const Stokes stokes, const glm::dmat3 rotation) {
    return rotation * ElectricField(stokesToLocalElectricField(stokes), complex::Complex{0, 0});
}

RAYX_FN_ACC
inline ElectricField electricFieldToStokes(const ElectricField field, const glm::dvec3 forward) {
    return localElectricFieldToStokes(globalToLocalElectricField(field, forward));
}

RAYX_FN_ACC
inline ElectricField electricFieldToStokes(const ElectricField field, const glm::dvec3 forward, const glm::dvec3 up) {
    return localElectricFieldToStokes(globalToLocalElectricField(field, forward, up));
}

}  // namespace RAYX