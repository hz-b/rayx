#include "Rays.h"

#include "Debug/Instrumentor.h"

namespace RAYX {

RAYX_API std::vector<std::string> getRayAttrNames() {
#define X(type, name, flag, map) #name,
    return std::vector<std::string>{RAYX_X_MACRO_RAY_ATTR};
#undef X
}

RayAttrMask rayAttrStringsToRayAttrMask(const std::vector<std::string>& strings) {
    auto stringToAttr = [](const std::string& str) -> RayAttrMask {
#define X(type, name, flag, map) \
    if (str == #name) return RayAttrMask::flag;
        RAYX_X_MACRO_RAY_ATTR
#undef X
        std::cerr << "error: failed to parse format string: unknown token: '" << str << "'";
        std::exit(1);
        return RayAttrMask::None;
    };

    auto attr = RayAttrMask::None;

    for (const auto& str : strings) { attr |= stringToAttr(str); }

    return attr;
}

}  // namespace RAYX
