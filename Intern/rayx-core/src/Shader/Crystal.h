#pragma once

#include <cmath>

#include "Complex.h"
#include "Constants.h"
#include "Core.h"
#include "Utils.h"

namespace RAYX {

// **********************************************************
// Function to calculate the local normal incidence angle theta
// from the direction cosines of the surface normal and the ray.
// The **negative** normal vector is used.
// Input:
//   al, am, an - direction cosines of the surface normal
//   fx, fy, fz - direction cosines of the incoming ray
// Output:
//   returns theta - the angle of incidence in radians
// **********************************************************
RAYX_FN_ACC double getTheta(Ray r, glm::dvec3 normal, double offsetAngle) {
    double al = normal[0];
    double am = normal[1];
    double an = normal[2];

    double fx = r.m_direction[0];
    double fy = r.m_direction[1];
    double fz = r.m_direction[2];

    // Normalize incoming ray vector
    double fn = complex::sqrt(fx * fx + fy * fy + fz * fz);
    if (fn == 0.0) fn = 1.0;
    fx /= fn;
    fy /= fn;
    fz /= fn;

    // Dot product of normalized ray and surface normal
    double ar = fx * al + fy * am + fz * an;

    // Clamp ar to the range [-1.0, 1.0] since std::clamp is not supported in CUDA
    if (ar < -1.0) {
        ar = -1.0;
    } else if (ar > 1.0) {
        ar = 1.0;
    }

    double theta = complex::acos(ar) - PI / 2;
    theta = theta + offsetAngle;
    return theta;  // TODO Fanny check how to correct this
}

/// @brief Computes the bragg angle
/// @param energy photonenergy of the ray
/// @param d lattice spacing*2
/// @param order Diffraction order
/// @return the bragg angle theta (rad)
RAYX_FN_ACC double getBraggAngle(double energy, double dSpacing2) {
    int order = 1;
    double wavelength = hvlam(energy);
    double theta_factor = (order * wavelength) / dSpacing2;

    // Check for physical validity
    if (theta_factor > 1.0) {
        return -1.0;  // No reflection possible
    }

    double theta = asin(theta_factor);  // In radians
    return theta;
}

/// @brief Computes the asymmetry factor b from the Bragg angle and surface tilt angle
/// @param braggAngle Bragg angle θ_B (in radians)
/// @param alpha Tilt between surface and diffraction planes α (in radians) (offset Angle)
/// @return Asymmetry factor b (dimensionless)
RAYX_FN_ACC double getAsymmetryFactor(double braggAngle, double alpha) {
    double numerator = complex::sin(braggAngle - alpha);
    double denominator = complex::sin(braggAngle + alpha);

    if (denominator == 0.0) return 0.0;  // avoid division by zero

    return numerator / denominator;
}

/// @brief Computes the diffraction prefactor Γ used in dynamical X-ray diffraction theory.
/// Defined in Batterman & Cole (1964), p. 685.
/// @param wavelength X-ray wavelength (in nm)
/// @param unitCellVolume Unit cell volume (in nm³)
/// @return Diffraction prefactor Γ
RAYX_FN_ACC double getDiffractionPrefactor(double wavelength, double unitCellVolume) {
    // Avoid division by zero
    if (wavelength <= 0.0 || unitCellVolume <= 0.0) {
        return 0.0;
    }
    double result = (ELECTRON_RADIUS * wavelength * wavelength) / PI / unitCellVolume;
    return result;
}

/// @brief Computes the normalized angular deviation parameter η (Eta) for dynamical X-ray diffraction.
///
///        This implementation follows Equation (32) from
///        Batterman & Cole (1964), p. 690
///
/// @param theta                Angle of incidence relative to the crystal surface (radians)
/// @param bragg                Bragg angle (radians)
/// @param asymmetry            Asymmetry factor
/// @param structureFactorReFH Real part of structure factor
/// @param structureFactorImFH Imaginary part of structure factor
/// @param structureFactorReFHC Real part of structure factor
/// @param structureFactorImFHC Imaginary part of structure factor
/// @param structureFactorReF0 Real part of structure factor
/// @param structureFactorImF0 Imaginary part of structure factor
/// @param polFactor            Polarization factor
/// @param gamma                Diffraction prefactor
/// @return                     Complex η parameter
RAYX_FN_ACC complex::Complex computeEta(double theta, double bragg, double asymmetry, double structureFactorReFH, double structureFactorImFH,
                                        double structureFactorReFHC, double structureFactorImFHC, double structureFactorReF0,
                                        double structureFactorImF0, double polFactor, double gamma) {
    // Calculate numerator terms
    complex::Complex top_term1 = asymmetry * (theta - bragg) * sin(2.0 * theta);
    complex::Complex top_term2 = 0.5 * gamma * complex::Complex(structureFactorReF0, structureFactorImF0) * (1.0 - asymmetry);
    complex::Complex top = top_term1 + top_term2;

    // Calculate denominator terms
    double bottom_term1 = gamma * polFactor;
    double bottom_term2 = sqrt(fabs(asymmetry));
    complex::Complex FH(structureFactorReFH, structureFactorImFH);
    complex::Complex FHC(structureFactorReFHC, structureFactorImFHC);
    complex::Complex bottom_term3 = sqrt(FH * FHC);

    complex::Complex bottom = bottom_term1 * bottom_term2 * bottom_term3;

    // Final eta calculation
    complex::Complex eta = top / bottom;
    return eta;
}

/// @brief Computes the reflection coefficient R based on eta
/// This function is based on Equation (103) from
/// Batterman & Cole (1964), p. 706,
///
/// @param eta                          Complex deviation parameter
/// @param structureFactorReFH          Real part of structure factor
/// @param structureFactorImFH          Imaginary part of structure factor
/// @param structureFactorReFHC         Real part of structure factor
/// @param structureFactorImFHC         Imaginary part of structure factor
/// @return                             Complex reflection coefficient R
RAYX_FN_ACC complex::Complex computeR(complex::Complex eta, double structureFactorReFH, double structureFactorImFH, double structureFactorReFHC,
                                      double structureFactorImFHC) {
    complex::Complex one(1.0, 0.0);
    complex::Complex FH(structureFactorReFH, structureFactorImFH);
    complex::Complex FHC(structureFactorReFHC, structureFactorImFHC);

    if (complex::real(eta) > 0.0) {
        return (eta - sqrt(eta * eta - one)) * sqrt(FH / FHC);
    } else {
        return (eta + sqrt(eta * eta - one)) * sqrt(FH / FHC);
    }
}

}  // namespace RAYX