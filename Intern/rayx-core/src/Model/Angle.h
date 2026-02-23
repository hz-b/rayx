#pragma once

#include "Design/Angle.h"

namespace rayx::detail {

inline double toModel(const Deg deg) { return toRad(deg).value(); }
inline double toModel(const Rad rad) { return rad.value(); }
inline double toModel(const Angle angle) {
    return std::visit([](auto arg) { return toModel(arg); }, angle);
}

}  // namespace rayx::detail
