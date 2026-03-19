#pragma once

#ifndef GLM_FORCE_XYZW_ONLY
#error "GLM_FORCE_XYZW_ONLY must be defined. This is required to enable default construction of LocalElectricField and ElectricField"
#endif

#include <glm/glm.hpp>
#include <numbers>

#include "Adapt.h"
#include "Constants.h"

////////////////////////////////////////////////////////////
// type definitions
////////////////////////////////////////////////////////////

namespace rayx {

using LocalElectricField = glm::tvec2<std::complex<double>>;
using ElectricField      = glm::tvec3<std::complex<double>>;
static_assert(std::is_default_constructible_v<LocalElectricField>);
static_assert(std::is_default_constructible_v<ElectricField>);

/// specialize type trait `LocalElectricField_T` for std::complex
template <>
struct detail::LocalElectricField_T<std::complex> {
    using type = LocalElectricField;
};

/// specialize type trait `ElectricField_T` for std::complex
template <>
struct detail::ElectricField_T<std::complex> {
    using type = ElectricField;
};

}  // namespace rayx

////////////////////////////////////////////////////////////
// utility functions
////////////////////////////////////////////////////////////

namespace rayx {

template <template <typename> typename TComplex>
RAYX_FN_ACC inline double intensity(const LocalElectricField_t<TComplex> field) {
    const auto mag = detail::adapt<TComplex>::abs(field);
    return glm::dot(mag, mag);
}

template <template <typename> typename TComplex>
RAYX_FN_ACC inline double intensity(const ElectricField_t<TComplex> field) {
    const auto mag = detail::adapt<TComplex>::abs(field);
    return glm::dot(mag, mag);
}

/**
 * Advances an electric field propagating along a given optical path length.
 * @param field Electric field of incident photon
 * @param waveLength Wavelength of incident photon (in nanometers)
 * @param opticalPathLength Optical path length traveled (in millimeters)
 * @return Advanced electric field
 */
template <template <typename> typename TComplex>
RAYX_FN_ACC inline ElectricField_t<TComplex> advanceElectricField(const ElectricField_t<TComplex> field, double waveLength,
                                                                  const double opticalPathLength) {
    // bring wavelength from nanometers into millimeters
    waveLength /= 1e6;

    // compute wave number (2π / λ), since opticalPathLength already includes IOR
    const double waveNumber = 2.0 * std::numbers::pi / waveLength;

    // reduce the distance modulo wavelength to avoid large angle errors
    const double reducedDistance = std::fmod(opticalPathLength, waveLength);

    // compute the phase shift
    const double deltaPhi = waveNumber * reducedDistance;

    // apply the phase shift as a complex exponential
    const auto phaseShift = detail::adapt<TComplex>::exp(TComplex<double>(0.0, deltaPhi));

    return field * phaseShift;
}

}  // namespace rayx

////////////////////////////////////////////////////////////
// std::ostream operator<< overloads
////////////////////////////////////////////////////////////

#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const rayx::LocalElectricField& field) {
    return os << "{Ex=" << field.x << ", Ey=" << field.y << "}";
}

inline std::ostream& operator<<(std::ostream& os, const rayx::ElectricField& field) {
    return os << "{Ex=" << field.x << ", Ey=" << field.y << ", Ez=" << field.z << "}";
}

////////////////////////////////////////////////////////////
// std::formatter specializations
////////////////////////////////////////////////////////////

// TODO: implement
