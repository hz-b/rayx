#pragma once

#include "Complex.h"
#include "Constants.h"

namespace RAYX {

using Stokes             = glm::dvec4;
using ElectricField      = cvec3;
using LocalElectricField = cvec2;

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

/**
 * Advances an electric field propagating along a given optical path length.
 * @param field Electric field of incident photon
 * @param waveLength Wavelength of incident photon (in nanometers)
 * @param opticalPathLength Optical path length traveled (in millimeters)
 * @return Advanced electric field
 */
RAYX_FN_ACC
inline ElectricField advanceElectricField(const ElectricField field, double waveLength, const double opticalPathLength) {
    // bring wavelength from nanometers into millimeters
    waveLength /= 1e6;

    // compute wave number (2π / λ), since opticalPathLength already includes IOR
    const double waveNumber = 2.0 * PI / waveLength;

    // reduce the distance modulo wavelength to avoid large angle errors
    const double reducedDistance = std::fmod(opticalPathLength, waveLength);

    // compute the phase shift
    const double deltaPhi = waveNumber * reducedDistance;

    // apply the phase shift as a complex exponential
    const auto phaseShift = complex::exp(complex::Complex(0.0, deltaPhi));

    return field * phaseShift;
}

/*
 *  rotation of electric field
 */

struct RotationBase {
    glm::dvec3 right;
    glm::dvec3 up;
    glm::dvec3 forward;
};

// Computes a base given a forward vector
// A convention for the up and right vectors is implemented, making this function well defined and for all forward directions
// TODO(Sven): this convention should be exchanged with one that does not branch
RAYX_FN_ACC
inline RotationBase forwardVectorToBaseConvention(const glm::dvec3 forward) {
    glm::dvec3 up;
    glm::dvec3 right;

    // test if the forward vector is close to being vertical
    const auto close_to_vertical = glm::abs(glm::dot(forward, glm::dvec3(0, 1, 0))) > .5;

    // if the forward vector is not close to being vertical, we initialize the up vector to (0, 1, 0)
    if (!close_to_vertical) {
        up    = glm::dvec3(0, 1, 0);
        right = glm::normalize(glm::cross(up, forward));
        up    = glm::normalize(glm::cross(forward, right));
    }

    // otherwise initialize the right vector to (1, 0, 0)
    else {
        right = glm::dvec3(1, 0, 0);
        up    = glm::normalize(glm::cross(forward, right));
        right = glm::normalize(glm::cross(up, forward));
    }

    return RotationBase{
        .right   = right,
        .up      = up,
        .forward = forward,
    };
}

// Computes a rotation matrix given a forward vector. This matrix can be used to align an object with a direction
RAYX_FN_ACC
inline glm::dmat3 rotationMatrixWithBaseConvention(const glm::dvec3 forward) {
    const auto base = forwardVectorToBaseConvention(forward);
    return glm::dmat3(base.right, base.up, base.forward);
}

RAYX_FN_ACC
inline glm::dmat3 rotationMatrix(const glm::dvec3 forward, const glm::dvec3 up) {
    const auto right = glm::cross(up, forward);
    return glm::dmat3(right, up, forward);
}

/*
 *  conversion between local and global electric field
 */

RAYX_FN_ACC
inline ElectricField localToGlobalElectricFieldWithBaseConvention(const LocalElectricField localField, const glm::dvec3 forward) {
    return rotationMatrixWithBaseConvention(forward) * ElectricField(localField, complex::Complex{0, 0});
}

RAYX_FN_ACC
inline ElectricField localToGlobalElectricField(const LocalElectricField localField, const glm::dvec3 forward, const glm::dvec3 up) {
    return rotationMatrix(forward, up) * ElectricField(localField, complex::Complex{0, 0});
}

RAYX_FN_ACC
inline ElectricField localToGlobalElectricField(const LocalElectricField localField, const glm::dmat3 rotation) {
    return rotation * ElectricField(localField, complex::Complex{0, 0});
}

RAYX_FN_ACC
inline LocalElectricField globalToLocalElectricFieldWithBaseConvention(const ElectricField field, const glm::dvec3 forward) {
    return glm::transpose(rotationMatrixWithBaseConvention(forward)) * field;
}

RAYX_FN_ACC
inline LocalElectricField globalToLocalElectricField(const ElectricField field, const glm::dvec3 forward, const glm::vec3 up) {
    return glm::transpose(rotationMatrix(forward, up)) * field;
}

RAYX_FN_ACC
inline LocalElectricField globalToLocalElectricField(const ElectricField field, const glm::dmat3 rotation) { return rotation * field; }

/*
 *  conversion between stokes and electric field
 */

RAYX_FN_ACC
inline Stokes localElectricFieldToStokes(const LocalElectricField field) {
    const auto mag   = complex::abs(field);
    const auto theta = complex::arg(field);

    return Stokes(mag.x * mag.x + mag.y * mag.y, mag.x * mag.x - mag.y * mag.y, 2.0 * mag.x * mag.y * glm::cos(theta.x - theta.y),
                  2.0 * mag.x * mag.y * glm::sin(theta.x - theta.y));
}

RAYX_FN_ACC
inline LocalElectricField stokesToLocalElectricField(const Stokes stokes) {
    const auto x_real  = glm::sqrt((stokes.x + stokes.y) / 2.0);
    const auto y_mag   = glm::sqrt((stokes.x - stokes.y) / 2.0);
    const auto y_theta = -1.0 * glm::atan(stokes.w, stokes.z);
    const auto y       = complex::polar(y_mag, y_theta);
    return LocalElectricField({x_real, 0}, y);
}

RAYX_FN_ACC
inline ElectricField stokesToElectricFieldWithBaseConvention(const Stokes stokes, const glm::dvec3 forward) {
    return localToGlobalElectricFieldWithBaseConvention(stokesToLocalElectricField(stokes), forward);
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
inline ElectricField electricFieldToStokesWithBaseConvention(const ElectricField field, const glm::dvec3 forward) {
    return localElectricFieldToStokes(globalToLocalElectricFieldWithBaseConvention(field, forward));
}

RAYX_FN_ACC
inline ElectricField electricFieldToStokes(const ElectricField field, const glm::dvec3 forward, const glm::dvec3 up) {
    return localElectricFieldToStokes(globalToLocalElectricField(field, forward, up));
}

RAYX_FN_ACC
inline ElectricField electricFieldToStokes(const ElectricField field, const glm::dmat3 rotation) {
    return localElectricFieldToStokes(globalToLocalElectricField(field, rotation));
}

}  // namespace RAYX
