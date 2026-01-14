#pragma once

namespace rayx::design {

struct SlopeError {
    double saggital;    ///< Slope error in the sagittal direction, aka `slopeErrorX`.
    double meridional;  ///< Slope error in the meridional direction, aka `slopeErrorZ`.
    // TODO: implement these features in the future
    // double thermalDistortionAmp;     ///< Amplitude of thermal distortion (unused).
    // double thermalDistortionSigmaX;  ///< Sigma of thermal distortion in X direction (unused).
    // double thermalDistortionSigmaZ;  ///< Sigma of thermal distortion in Z direction (unused).
    // double cylindricalBowingAmp;     ///< Amplitude of cylindrical bowing (unused).
    // double cylindricalBowingRadius;  ///< Radius of cylindrical bowing (unused).
};

}  // namespace rayx::design
