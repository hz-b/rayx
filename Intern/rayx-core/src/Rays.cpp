#include "Rays.h"

#include "Debug/Instrumentor.h"

namespace RAYX {

RAYX_API std::vector<std::string> getRayAttrNames() {
#define X(type, name, flag, map) #name,
    return std::vector<std::string>{RAYX_X_MACRO_RAY_ATTR};
#undef X
}

RayAttrFlag rayAttrStringsToRayAttrMask(const std::vector<std::string>& strings) {
    auto stringToAttr = [](const std::string& str) -> RayAttrFlag {
#define X(type, name, flag, map) \
    if (str == #name) return RayAttrFlag::flag;
        RAYX_X_MACRO_RAY_ATTR
#undef X
        std::cerr << "error: failed to parse format string: unknown token: '" << str << "'";
        std::exit(1);
        return RayAttrFlag::None;
    };

    auto attr = RayAttrFlag::None;

    for (const auto& str : strings) { attr |= stringToAttr(str); }

    return attr;
}

}  // namespace RAYX
