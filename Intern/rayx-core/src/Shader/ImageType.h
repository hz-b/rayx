#pragma once

#include "Core.h"

namespace RAYX {

// The ImageType is a parameter within the RZP.
// It's different possible values are listed below.
// These numbers are chosen to be compatible with RAY-UI.
constexpr int IT_POINT2POINT = 0;
constexpr int IT_POINT2HORIZONTAL_LINE = 4;
constexpr int IT_POINT2HORIZONTAL_DIVERGENT_LINE = 5;
constexpr int IT_ASTIGMATIC2ASTIGMATIC = 12;

}  // namespace RAYX
