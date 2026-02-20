#pragma once

#include "Design/Angle.h"
#include "ToModel.h"

namespace rayx::detail {

inline double toModel(const Deg deg) { return toRad(deg).value(); }
inline double toModel(const Rad rad) { return rad.value(); }

}  // namespace rayx::detail
