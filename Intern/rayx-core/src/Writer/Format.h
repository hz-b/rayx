#pragma once

#include "Debug/Debug.h"
#include "RaySoA.h"

namespace RAYX {

inline RayAttrFlag formatStringToRayAttrFlag(const std::string& format) {
    auto stringToAttr = [](const std::string& str) -> RayAttrFlag {
#define X(type, name, flag, map) \
    if (str == #name) return RayAttrFlag::flag;
        RAYX_X_MACRO_RAY_ATTR
#undef X
        RAYX_EXIT << "error: failed to parse format string: unknown token: '" << str << "'";
        return RayAttrFlag::None;
    };

    auto attr = RayAttrFlag::None;
    auto ss = std::stringstream(format);
    std::string token;
    while (ss >> token) {
        attr |= stringToAttr(token);
    }

    return static_cast<RayAttrFlag>(attr);
}

}  // namespace RAYX
