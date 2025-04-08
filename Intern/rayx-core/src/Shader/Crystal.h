#pragma once

#include "Core.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

namespace RAYX {

// **********************************************************
// Function to calculate the local normal incidence angle theta
// from the direction cosines of the surface normal and the ray.
// The **negative** normal vector is used.
// Input:
//   al, am, an - direction cosines of the surface normal
//   fx, fy, fz - direction cosines of the incoming ray
// Output:
//   returns theta - the angle of incidence in degrees
// **********************************************************

RAYX_FN_ACC double getTheta(Ray r, glm::dvec3 normal) {


    double al = normal[0];
    double am = normal[1];
    double an = normal[2];
    
    double fx = r.m_direction[0];
    double fy = r.m_direction[0];
    double fz = r.m_direction[2];
    
    // Normalize incoming ray vector
    double fn = std::sqrt(fx * fx + fy * fy + fz * fz);
    if (fn == 0.0) fn = 1.0;
    fx /= fn;
    fy /= fn;
    fz /= fn;

    // Dot product of normalized ray and surface normal
    double ar = fx * al + fy * am + fz * an;
    ar = std::clamp(ar, -1.0, 1.0);

    // Calculate incidence angle (in degrees)
    double theta = PI - std::acos(ar);
    theta = theta * 180.0 / PI;

    if (theta > 90.0) theta = 180.0 - theta;

    return theta;
}


/// @brief this function calculates the energy dependent bragg angle 
/// Bragg's law: n * λ = 2 * d * sin(θ)
/// @param energy photonenergy of the ray
/// @param d Interplanar spacing
/// @param order Diffraction order 
/// @return the bragg angle theta
RAYX_FN_ACC double getBraggAngle(double energy, double d, int order = 1) {
    
    double wavelength = hvlam(energy);
    double theta_factor = (order * wavelength) / (2.0 * d);

    // Check for physical validity
    if (theta_factor > 1.0) {
        return -1.0; // No reflection possible
    }

    double theta = asin(theta_factor); // In radians
    return theta;
}


/// @brief Computes the asymmetry factor b from the Bragg angle and surface tilt angle
/// @param braggAngle Bragg angle θ_B (in radians)
/// @param alpha Tilt between surface and diffraction planes α (in radians) (offset Angle)
/// @return Asymmetry factor b (dimensionless)
RAYX_FN_ACC double getAsymmetryFactor(double braggAngle, double alpha) {
    double numerator = std::sin(braggAngle - alpha);
    double denominator = std::sin(braggAngle + alpha);
    
    if (denominator == 0.0) return 0.0; // avoid division by zero
    
    return numerator / denominator;
}



/// @brief This function calculates the absorption parameter g for asymmetric reflections
///        using the dynamical theory of X-ray diffraction. It accounts for the different
///        path lengths due to asymmetry in incidence and reflection geometry.
///
/// @param absorptionCoefficient Linear absorption coefficient μ (in 1/m)
/// @param wavelength X-ray wavelength (in meters)
/// @param asymmetryFactor Asymmetry factor b (dimensionless)
/// @param braggAngle Bragg angle θ_B (in radians)
/// @param theta1 Angle between incident beam and crystal surface (in radians)
/// @param theta2 Angle between exit beam and crystal surface (in radians)
/// @return Absorption parameter g (dimensionless)
RAYX_FN_ACC double getAbsorptionParameterAsymmetric(double structureFactor0Imag, double preFactor, double polFactor, double asymmetryFactor) {
    
    double numerator = 0.5 * (1 + asymmetryFactor) * preFactor * structureFactor0Imag;
    double denominator = polFactor * sqrt(assymetryFactor) * abs(preFactor * std::real(structureFactorH));

    double g = numerator / denominator;

    return g;
}


/// @brief Computes the prefactor formerly GAMA used in calculating the absorption and dispersion
///        parameters in dynamical X-ray diffraction theory. The prefactor is derived from
///        the Fourier component of the crystal polarizability.
///
/// @param wavelength X-ray wavelength in meters
/// @param unitCellVolume Unit cell volume in cubic meters
/// @return Prefactor value (dimensionless)
RAYX_FN_ACC double getDiffractionPrefactor(double wavelength, double unitCellVolume) {

    // Avoid division by zero
    if (wavelength <= 0.0 || unitCellVolume <= 0.0) {
        return 0.0;
    }

    return - (ELECTRON_RADIUS * wavelength * wavelength) / (PI * unitCellVolume);
}


/// @brief Calculates the phase shift (dispersion) parameter k used in the dynamical theory
///        of X-ray diffraction. It represents the real part of the crystal polarizability
///        and contributes to the shape of the intrinsic rocking curve.
///
/// @param wavelength X-ray wavelength in meters (e.g., 1.54e-10 for CuKα)
/// @param structureFactor Complex structure factor F_H (unit: electrons)
/// @param unitCellVolume Unit cell volume in cubic meters
/// @return Phase shift parameter k (dimensionless)
RAYX_FN_ACC double getPhaseShiftParameter(double structureFactorReH, double structureFactorImH, double prefactor) {
    // Use only the real part for the phase shift parameter
    double k = prefactor * (structureFactorImH/structureFactorReH);

    return k;
}


/// @brief This function calculates the normalized angular deviation parameter W
///        used in dynamical X-ray diffraction theory.
///        W represents the deviation from the Bragg condition, normalized by the intrinsic
///        angular width of the reflection profile (rocking curve width).
///
///        This is used as input to the reflection profile function R(W).
///
/// @param theta Actual angle of incidence (in radians)
/// @param bragg Bragg angle for the diffraction (in radians)
/// @param intrinsicWidth Angular width (FWHM or full total reflection width) of the rocking curve (in radians)
/// @return Normalized deviation W (unitless)
RAYX_FN_ACC double computeW(double theta0, double bragg, double assymetryFactor, double structureFactorReH, double structureFactorRe0, double polFactor, double preFactor) {
    
    double summand1 = 0.5 * (1 + assymetryFactor) * preFactor * structureFactorRe0;
    double summand2 = assymetryFactor * (theta0 - bragg) * sin(2*bragg);
    double numerator = summand1 + summand2;
    
    double denominator = polFactor * sqrt(assymetryFactor) * abs(preFactor * structureFactorReH);

    double W = nummerator / denominator;
    return W;
}



/// @brief This function calculates the reflection intensity R(W) for X-ray diffraction
///        based on the dynamical theory of perfect crystals.
///        R(W) describes the reflectance as a function of angular deviation from the Bragg condition.
///
///        The formula is based on the intrinsic rocking curve from Kikuta (1971), and includes
///        polarization and absorption effects via the g and k parameters.
///
/// @param offsetAngle Normalized angular deviation from the Bragg angle (unitless)
/// @param g Ratio of imaginary to real part of the crystal polarizability (absorption term)
/// @param k Asymmetry-related shift in the dynamical interference profile
/// @return Reflectance R(W) in the range [0, 1]
RAYX_FN_ACC double computeReflectionProfile(double angularDeviation, double absorptiong, double dispersionk) {
    // Precompute squared values
    double angularDeviation2 = angularDeviation * angularDeviation;
    double absorptiong2 = absorptiong * absorptiong;
    double dispersionk2 = dispersionk * dispersionk;

    // Part of the L expression (see Kikuta eqn. for L in dynamical theory)
    double diff = angularDeviation2 - absorptiong2 - 1.0 + dispersionk2;

    // First term inside outer sqrt
    double term1 = angularDeviation2 + absorptiong2;

    // Second term inside outer sqrt: interference-related broadening
    double term2 = std::sqrt(diff * diff + 4.0 * absorptiong2 * (angularDeviation - dispersionk) * (angularDeviation - dispersionk));

    // L(W): determines shape of the reflectance curve
    double L = std::sqrt(term1 + term2);

    // Reflectance R(W), clipped automatically by the model (R ∈ [0, 1])
    return 1.0 / (L * L - 1.0);
}





/// @brief This function calculates the intrinsic angular width (omega) of the rocking curve
///        for a perfect crystal using the dynamical theory of X-ray diffraction.
///        It is used to normalize angular deviations when computing W.
///
/// @param wavelength X-ray wavelength (in meters)
/// @param structureFactorMagnitude Magnitude of the structure factor |F_H| (unitless or in e− units)
/// @param unitCellVolume Unit cell volume of the crystal (in cubic meters)
/// @param braggAngle Bragg angle for the diffraction (in radians)
/// @return Intrinsic angular width omega (in radians)
RAYX_FN_ACC double getIntrinsicWidth(double wavelength, double structureFactorMagnitude, double unitCellVolume, double braggAngle) {
    // Avoid division by zero or invalid input
    if (unitCellVolume <= 0.0 || std::cos(braggAngle) == 0.0) {
        return 0.0;
    }

    // Intrinsic rocking curve width according to Kikuta et al. (1971)
    double omega = (wavelength * structureFactorMagnitude) / (M_PI * unitCellVolume * std::cos(braggAngle));

    return omega;
}



}