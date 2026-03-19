#pragma once

#include "Property.h"

namespace rayx {

// TODO: find sensible defaults for slope errors
// TODO: find sensible validation for slope errors
// TODO: require mandatory arguments in constructors

struct SlopeError {
    // Slope error in the sagittal direction, aka `slopeErrorX`.
    RAYX_PROPERTY(SlopeError, double, saggital) = 0.0;
    // Slope error in the sagittal direction, aka `slopeErrorZ`.
    RAYX_PROPERTY(SlopeError, double, meridional) = 0.0;

    // TODO: implement these features in the future
    // double thermalDistortionAmp;     ///< Amplitude of thermal distortion (unused).
    // double thermalDistortionSigmaX;  ///< Sigma of thermal distortion in X direction (unused).
    // double thermalDistortionSigmaZ;  ///< Sigma of thermal distortion in Z direction (unused).
    // double cylindricalBowingAmp;     ///< Amplitude of cylindrical bowing (unused).
    // double cylindricalBowingRadius;  ///< Radius of cylindrical bowing (unused).
};

}  // namespace rayx
