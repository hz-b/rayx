#include "Utils.h"

#include "Constants.h"
#include "Efficiency.h"

namespace RAYX {

/**
 * @brief Calculates the transmission through a single thin film layer using Fresnel equations.
 *
 * This function computes the intensity transmission coefficient (T) for a plane wave incident on 
 * a single dielectric or absorbing film, including interference effects.
 *
 * @param wavelength        Wavelength of light in nanometers.
 * @param thickness         Thickness of the film in nanometers.
 * @param n0                Complex refractive index of the incident medium (vakuum).
 * @param n1                Complex refractive index of the film layer.
 * @param incidentAngleRad  Angle of incidence in radians (from normal).
 * @return double           Intensity transmission coefficient (range 0 to 1).
 */
RAYX_FN_ACC
inline ComplexFresnelCoeffs computeTransmittance(
    double wavelength,         
    complex::Complex theta0,
    const complex::Complex& indexVacuum,
    const complex::Complex& indexMaterial,
    double thickness           
) {
    using complex::Complex;

    complex::Complex sinTheta1 = (indexVacuum / indexMaterial) * std::sin(theta0);
    complex::Complex theta1 = std::asin(sinTheta1);
    
    complex::Complex theta2 = theta0; // Austritt in Vakuum, Winkel gleich theta0

    // Fresnel-Koeffizienten (s- und p-Polarisation)
    auto rs = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (ni * std::cos(thetai) - nt * std::cos(thetat)) /
               (ni * std::cos(thetai) + nt * std::cos(thetat));
    };
    auto ts = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (2.0 * ni * std::cos(thetai)) /
               (ni * std::cos(thetai) + nt * std::cos(thetat));
    };
    auto rp = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (nt * std::cos(thetai) - ni * std::cos(thetat)) /
               (nt * std::cos(thetai) + ni * std::cos(thetat));
    };
    auto tp = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (2.0 * ni * std::cos(thetai)) /
               (nt * std::cos(thetai) + ni * std::cos(thetat));
    };

    // s-Polarisation
    complex::Complex r01s = rs(indexVacuum, indexMaterial, theta0, theta1);
    complex::Complex t01s = ts(indexVacuum, indexMaterial, theta0, theta1);
    complex::Complex r12s = rs(indexMaterial, indexVacuum, theta1, theta2);
    complex::Complex t12s = ts(indexMaterial, indexVacuum, theta1, theta2);

    // p-Polarisation
    complex::Complex r01p = rp(indexVacuum, indexMaterial, theta0, theta1);
    complex::Complex t01p = tp(indexVacuum, indexMaterial, theta0, theta1);
    complex::Complex r12p = rp(indexMaterial, indexVacuum, theta1, theta2);
    complex::Complex t12p = tp(indexMaterial, indexVacuum, theta1, theta2);

    // Phasenverschiebung
    complex::Complex delta = (2.0 * PI / wavelength) * indexMaterial * thickness * std::cos(theta1);
    complex::Complex phase = std::exp(complex::Complex(0, 1) * delta);

    // Gesamte Transmission s
    complex::Complex numeratorS = t01s * t12s * phase;
    complex::Complex denominatorS = 1.0 + r01s * r12s * phase * phase;
    complex::Complex ts_total = numeratorS / denominatorS;

    // Gesamte Transmission p
    complex::Complex numeratorP = t01p * t12p * phase;
    complex::Complex denominatorP = 1.0 + r01p * r12p * phase * phase;
    complex::Complex tp_total = numeratorP / denominatorP;

    return {.s = ts_total, .p = tp_total};
}

}