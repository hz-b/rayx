#pragma once

#include "Debug/Debug.h"
#include "RaySoA.h"

namespace RAYX {

inline RayAttrFlag formatStringToRayAttrFlag(const std::string& format) {
    auto stringToAttr = [](const std::string& str) -> RayAttrFlagType {
#define RAYX_X(type, name, flag, map) \
    if (str == #name) return flag;
        RAYX_X_MACRO_RAY_ATTR_PATH_ID
        RAYX_X_MACRO_RAY_ATTR
#undef RAYX_X
        RAYX_EXIT << "error: failed to parse format string: unknown token: '" << str << "'";
        return static_cast<RayAttrFlagType>(0);
    };

    auto attr = static_cast<RayAttrFlagType>(0);
    auto ss = std::stringstream(format);
    std::string token;
    while (ss >> token) {
        attr |= stringToAttr(token);
    }

    return static_cast<RayAttrFlag>(attr);
}

}  // namespace RAYX
