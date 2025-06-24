#pragma once

#include "Ray.h"

#include "Complex.h"
#include "Efficiency.h"


namespace RAYX {


RAYX_FN_ACC complex::Complex calcPhaseShift(const complex::Complex& ior, double thickness, double wavelength, const complex::Complex& angle);


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
RAYX_FN_ACC ComplexFresnelCoeffs computeTransmittanceFoil(double wavelength, complex::Complex incidentAngleRad, const complex::Complex& n0, const complex::Complex& n1, double thickness);


}
