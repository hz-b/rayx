#pragma once

#include "Ray.h"
#include "Throw.h"

namespace rayx {

/**
 * converts energy (eV) to wavelength (nm)
 * @param energy energy of a photon in eV. must not be 0
 * @return wavelength of photon in nm
 */
RAYX_FN_ACC inline double RAYX_API energyToWaveLength(double x) { return INV_NM_TO_EVOLT / x; }

/**
 * Convert photon wavelength (nm) to energy (eV)
 * @param wavelength of photon in nm. must not be 0
 * @return energy energy of a photon in eV
 */
RAYX_FN_ACC inline double waveLengthToEnergy(const double waveLength) { return INV_NM_TO_EVOLT / waveLength; }

RAYX_FN_ACC
inline void RAYX_API rayMatrixMult(const glm::dmat4& __restrict m, glm::dvec3& __restrict rayPosition, glm::dvec3& __restrict rayDirection) {
    rayPosition  = glm::dvec3(m * glm::dvec4(rayPosition, 1));
    rayDirection = glm::dvec3(m * glm::dvec4(rayDirection, 0));
}

RAYX_FN_ACC
inline void RAYX_API rayMatrixMult(const glm::dmat4& __restrict m, glm::dvec3& __restrict rayPosition, glm::dvec3& __restrict rayDirection,
                                   ElectricField& __restrict rayElectricField) {
    rayPosition      = glm::dvec3(m * glm::dvec4(rayPosition, 1));
    rayDirection     = glm::dvec3(m * glm::dvec4(rayDirection, 0));
    rayElectricField = glm::dmat3(m) * rayElectricField;
}

}  // namespace rayx
