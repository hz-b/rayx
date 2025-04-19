#pragma once

#include "Complex.h"
#include "Constants.h"

namespace RAYX {

using Stokes = glm::dvec4;
using ElectricField = cvec3;
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
