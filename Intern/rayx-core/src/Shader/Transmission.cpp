#include "Utils.h"

#include "Constants.h"
#include "Efficiency.h"

namespace RAYX {

RAYX_FN_ACC
complex::Complex calcPhaseShift(const complex::Complex& ior, double thickness, double wavelength, const complex::Complex& angle) {
    const complex::Complex delta = (2.0 * PI * ior * thickness * complex::cos(angle)) / wavelength;
    return complex::exp(complex::Complex(0.0, -1.0) * delta);
}

RAYX_FN_ACC
ComplexFresnelCoeffs computeTransmittanceFoil(
    double wavelength,         
    complex::Complex theta0,
    const complex::Complex& indexVacuum,
    const complex::Complex& indexMaterial,
    double thickness           
) {
    using complex::Complex;

    // Snell's Law
    Complex sinTheta1 = (indexVacuum / indexMaterial) * std::sin(theta0);
    Complex theta1 = std::asin(sinTheta1);

    Complex theta2 = theta0; // Austritt in Vakuum, Winkel gleich theta0

    // Fresnel-Koeffizienten (s- und p-Polarisation)
    auto rs = [](Complex ni, Complex nt, Complex thetai, Complex thetat) {
        return (ni * std::cos(thetai) - nt * std::cos(thetat)) /
               (ni * std::cos(thetai) + nt * std::cos(thetat));
    };
    auto ts = [](Complex ni, Complex nt, Complex thetai, Complex thetat) {
        return (2.0 * ni * std::cos(thetai)) /
               (ni * std::cos(thetai) + nt * std::cos(thetat));
    };
    auto rp = [](Complex ni, Complex nt, Complex thetai, Complex thetat) {
        return (nt * std::cos(thetai) - ni * std::cos(thetat)) /
               (nt * std::cos(thetai) + ni * std::cos(thetat));
    };
    auto tp = [](Complex ni, Complex nt, Complex thetai, Complex thetat) {
        return (2.0 * ni * std::cos(thetai)) /
               (nt * std::cos(thetai) + ni * std::cos(thetat));
    };

    // s-Polarisation
    Complex r01s = rs(indexVacuum, indexMaterial, theta0, theta1);
    Complex t01s = ts(indexVacuum, indexMaterial, theta0, theta1);
    Complex r12s = rs(indexMaterial, indexVacuum, theta1, theta2);
    Complex t12s = ts(indexMaterial, indexVacuum, theta1, theta2);

    // p-Polarisation
    Complex r01p = rp(indexVacuum, indexMaterial, theta0, theta1);
    Complex t01p = tp(indexVacuum, indexMaterial, theta0, theta1);
    Complex r12p = rp(indexMaterial, indexVacuum, theta1, theta2);
    Complex t12p = tp(indexMaterial, indexVacuum, theta1, theta2);

    // Phasenverschiebung
    Complex delta = (2.0 * PI / wavelength) * indexMaterial * thickness * std::cos(theta1);
    Complex phase = std::exp(Complex(0, delta.real()));  // Ignoriere imag(delta) für einfache Modelle

    // Gesamte Transmission s
    Complex numeratorS = t01s * t12s * phase;
    Complex denominatorS = 1.0 + r01s * r12s * phase * phase;
    Complex ts_total = numeratorS / denominatorS;

    // Gesamte Transmission p
    Complex numeratorP = t01p * t12p * phase;
    Complex denominatorP = 1.0 + r01p * r12p * phase * phase;
    Complex tp_total = numeratorP / denominatorP;

    return {.s = ts_total, .p = tp_total};
}



}
