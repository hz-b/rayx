#pragma once

/**
 * @brief Structure to represent slope error parameters.
 */
struct SlopeError {
    double m_sag;                      ///< Slope error in the sagittal direction, aka `slopeErrorX`.
    double m_mer;                      ///< Slope error in the meridional direction, aka `slopeErrorZ`.
    double m_thermalDistortionAmp;     ///< Amplitude of thermal distortion (unused).
    double m_thermalDistortionSigmaX;  ///< Sigma of thermal distortion in X direction (unused).
    double m_thermalDistortionSigmaZ;  ///< Sigma of thermal distortion in Z direction (unused).
    double m_cylindricalBowingAmp;     ///< Amplitude of cylindrical bowing (unused).
    double m_cylindricalBowingRadius;  ///< Radius of cylindrical bowing (unused).
};