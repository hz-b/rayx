#include "Constants.h"
#include "Efficiency.h"
#include "Utils.h"

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
inline ComplexFresnelCoeffs computeTransmittance(double wavelength, complex::Complex theta0, const complex::Complex& indexVacuum,
                                                 const complex::Complex& indexMaterial, double thickness) {
    using complex::Complex;

    complex::Complex sinTheta1 = (indexVacuum / indexMaterial) * complex::sin(theta0);
    complex::Complex theta1    = complex::asin(sinTheta1);

    complex::Complex theta2 = theta0;  // Austritt in Vakuum, Winkel gleich theta0

    // Fresnel-Koeffizienten (s- und p-Polarisation)
    auto rs = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (ni * complex::cos(thetai) - nt * complex::cos(thetat)) / (ni * complex::cos(thetai) + nt * complex::cos(thetat));
    };
    auto ts = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (2.0 * ni * complex::cos(thetai)) / (ni * complex::cos(thetai) + nt * complex::cos(thetat));
    };
    auto rp = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (nt * complex::cos(thetai) - ni * complex::cos(thetat)) / (nt * complex::cos(thetai) + ni * complex::cos(thetat));
    };
    auto tp = [](complex::Complex ni, complex::Complex nt, complex::Complex thetai, complex::Complex thetat) {
        return (2.0 * ni * complex::cos(thetai)) / (nt * complex::cos(thetai) + ni * complex::cos(thetat));
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
    complex::Complex delta = (2.0 * PI / wavelength) * indexMaterial * thickness * complex::cos(theta1);
    complex::Complex phase = complex::exp(complex::Complex(0, 1) * delta);

    // Gesamte Transmission s
    complex::Complex numeratorS   = t01s * t12s * phase;
    complex::Complex denominatorS = 1.0 + r01s * r12s * phase * phase;
    complex::Complex ts_total     = numeratorS / denominatorS;

    // Gesamte Transmission p
    complex::Complex numeratorP   = t01p * t12p * phase;
    complex::Complex denominatorP = 1.0 + r01p * r12p * phase * phase;
    complex::Complex tp_total     = numeratorP / denominatorP;

    return {.s = ts_total, .p = tp_total};
}


// compute transmittance for entrance or exit of a coating on a substrate
RAYX_FN_ACC ComplexFresnelCoeffs computeTransmittanceOnBarrier(double wavelength, complex::Complex theta0, const complex::Complex& indexEntry,
                                                                const complex::Complex& indexExit, double thickness) {
                                                                    
}
}  // namespace RAYX