#pragma once

#include "Core.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

/// @brief a
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
    double fn = std::sqrt(fx * fx + fy * fy + fz * fz);
    if (fn == 0.0) fn = 1.0;
    fx /= fn;
    fy /= fn;
    fz /= fn;

    // Dot product of normalized ray and surface normal
    double ar = fx * al + fy * am + fz * an;
    ar = std::clamp(ar, -1.0, 1.0);

    double theta = std::acos(ar) - PI/2;
    theta = theta + offsetAngle;
    return theta;                             //TODO Fanny check how to correct this
}

/// @brief this function calculates the energy dependent bragg angle 
/// Bragg's law: n * λ = 2 * d * sin(θ)
/// @param energy photonenergy of the ray
/// @param d Interplanar spacing2
/// @param order Diffraction order 
/// @return the bragg angle theta
RAYX_FN_ACC double getBraggAngle(double energy, double dSpacing2) {
    
    int order = 1;
    double wavelength = hvlam(energy);
    double theta_factor = (order * wavelength) / dSpacing2;

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
    double eleradius = ELECTRON_RADIUS * 1.e9 ;
    double result = (eleradius * wavelength * wavelength) / PI / unitCellVolume; 
    return result;
}


/// @brief Computes the normalized angular deviation parameter eta (W) for dynamical X-ray diffraction.
///        This implements the calculation from the original FORTRAN DARWIN subroutine.
///
/// @param theta       Angle of incidence relative to surface (radians)
/// @param thetaB      Bragg angle (radians)
/// @param asymmetry   Asymmetry factor (b)
/// @param FH_real     Real part of the complex structure factor F_h
/// @param FH_imag     Imaginary part of the complex structure factor F_h
/// @param FHC_real    Real part of the complex structure factor F_h_bar
/// @param FHC_imag    Imaginary part of the complex structure factor F_h_bar
/// @param F0_real     Real part of the structure factor F_0
/// @param F0_imag     Imaginary part of the structure factor F_0
/// @param polFactor   Polarization factor (1 for s-pol, |cos(2θ_B)| for p-pol)
/// @param gamma       Scaling factor (2.818e-6 * wavelength^2 / (π * unit cell volume))
/// @return            Complex value of eta
RAYX_FN_ACC std::complex<double> computeEta(double theta, double thetaB, double asymmetry, 
                                           double FH_real, double FH_imag, 
                                           double FHC_real, double FHC_imag,
                                           double F0_real, double F0_imag,
                                           double polFactor, double gamma) {
    // Calculate numerator terms
    std::complex<double> top_term1 = asymmetry * (theta - thetaB) * sin(2.0 * theta);
    std::complex<double> top_term2 = 0.5 * gamma * std::complex<double>(F0_real, F0_imag) * (1.0 - asymmetry);
    std::complex<double> top = top_term1 + top_term2;

    // Calculate denominator terms
    double bottom_term1 = gamma * polFactor;
    double bottom_term2 = sqrt(fabs(asymmetry));
    std::complex<double> FH(FH_real, FH_imag);
    std::complex<double> FHC(FHC_real, FHC_imag);
    std::complex<double> bottom_term3 = sqrt(FH * FHC);
    
    std::complex<double> bottom = bottom_term1 * bottom_term2 * bottom_term3;

    // Final eta calculation
    std::complex<double> eta = top / bottom;
    return eta;
}

/// @brief Computes the reflection coefficient R based on eta
/// @param eta         Complex eta value
/// @param FH_real     Real part of F_h
/// @param FH_imag     Imaginary part of F_h
/// @param FHC_real    Real part of F_h_bar
/// @param FHC_imag    Imaginary part of F_h_bar
/// @return            Complex reflection coefficient R
RAYX_FN_ACC std::complex<double> computeR(std::complex<double> eta, 
                                         double FH_real, double FH_imag,
                                         double FHC_real, double FHC_imag) {
    std::complex<double> one(1.0, 0.0);
    std::complex<double> FH(FH_real, FH_imag);
    std::complex<double> FHC(FHC_real, FHC_imag);
    
    if (std::real(eta) > 0.0) {
        auto R = (eta - sqrt(eta * eta - one)) * sqrt(FH / FHC);
        return R;
    } else {
        auto R = (eta + sqrt(eta * eta - one)) * sqrt(FH / FHC);
        return R;
    }
}


}