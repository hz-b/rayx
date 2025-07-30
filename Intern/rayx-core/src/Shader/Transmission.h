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
inline ComplexFresnelCoeffs computeTransmittance(double wavelength, std::complex<double> theta0, const std::complex<double>& indexVacuum,
                                                 const std::complex<double>& indexMaterial, double thickness) {
    std::complex<double> sinTheta1 = (indexVacuum / indexMaterial) * std::sin(theta0);
    std::complex<double> theta1 = std::asin(sinTheta1);

    std::complex<double> theta2 = theta0;  // Austritt in Vakuum, Winkel gleich theta0

    // Fresnel-Koeffizienten (s- und p-Polarisation)
    auto rs = [](std::complex<double> ni, std::complex<double> nt, std::complex<double> thetai, std::complex<double> thetat) {
        return (ni * std::cos(thetai) - nt * std::cos(thetat)) / (ni * std::cos(thetai) + nt * std::cos(thetat));
    };
    auto ts = [](std::complex<double> ni, std::complex<double> nt, std::complex<double> thetai, std::complex<double> thetat) {
        return (2.0 * ni * std::cos(thetai)) / (ni * std::cos(thetai) + nt * std::cos(thetat));
    };
    auto rp = [](std::complex<double> ni, std::complex<double> nt, std::complex<double> thetai, std::complex<double> thetat) {
        return (nt * std::cos(thetai) - ni * std::cos(thetat)) / (nt * std::cos(thetai) + ni * std::cos(thetat));
    };
    auto tp = [](std::complex<double> ni, std::complex<double> nt, std::complex<double> thetai, std::complex<double> thetat) {
        return (2.0 * ni * std::cos(thetai)) / (nt * std::cos(thetai) + ni * std::cos(thetat));
    };

    // s-Polarisation
    std::complex<double> r01s = rs(indexVacuum, indexMaterial, theta0, theta1);
    std::complex<double> t01s = ts(indexVacuum, indexMaterial, theta0, theta1);
    std::complex<double> r12s = rs(indexMaterial, indexVacuum, theta1, theta2);
    std::complex<double> t12s = ts(indexMaterial, indexVacuum, theta1, theta2);

    // p-Polarisation
    std::complex<double> r01p = rp(indexVacuum, indexMaterial, theta0, theta1);
    std::complex<double> t01p = tp(indexVacuum, indexMaterial, theta0, theta1);
    std::complex<double> r12p = rp(indexMaterial, indexVacuum, theta1, theta2);
    std::complex<double> t12p = tp(indexMaterial, indexVacuum, theta1, theta2);

    // Phasenverschiebung
    std::complex<double> delta = (2.0 * PI / wavelength) * indexMaterial * thickness * std::cos(theta1);
    std::complex<double> phase = std::exp(std::complex<double>(0, 1) * delta);

    // Gesamte Transmission s
    std::complex<double> numeratorS = t01s * t12s * phase;
    std::complex<double> denominatorS = 1.0 + r01s * r12s * phase * phase;
    std::complex<double> ts_total = numeratorS / denominatorS;

    // Gesamte Transmission p
    std::complex<double> numeratorP = t01p * t12p * phase;
    std::complex<double> denominatorP = 1.0 + r01p * r12p * phase * phase;
    std::complex<double> tp_total = numeratorP / denominatorP;

    return {.s = ts_total, .p = tp_total};
}

}  // namespace RAYX
