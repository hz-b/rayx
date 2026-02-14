#pragma once

#include "Design/Angle.h"

namespace rayx::detail {

inline double toDevice(const Deg degrees) { return toRad(degrees).value(); }
inline double toDevice(const Rad radians) { return toRad(radians).value(); }
inline double toDevice(const Angle angle) { return toRad(angle).value(); }

}  // namespace rayx::detail::host
