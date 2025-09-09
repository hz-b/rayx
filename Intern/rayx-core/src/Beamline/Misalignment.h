#pragma once

#include "Angle.h"

namespace RAYX {

// An error in position and orientation that an object might have.
struct Misalignment {
    double m_translationXerror;
    double m_translationYerror;
    double m_translationZerror;
    Rad m_rotationXerror;
    Rad m_rotationYerror;
    Rad m_rotationZerror;
};

}  // namespace RAYX
