#include "RayAttrMask.h"

#include <bitset>

namespace rayx {

int countSetBits(const RayAttrMask mask) {
    return std::bitset<static_cast<std::underlying_type_t<RayAttrMask>>(RayAttrMask::RayAttrMaskCount)>(
               static_cast<std::underlying_type_t<RayAttrMask>>(mask))
        .count();
}

bool isFlag(const RayAttrMask attr) { return countSetBits(attr) == 1; }

std::string to_string(const RayAttrMask attr) {
    return std::bitset<static_cast<std::underlying_type_t<RayAttrMask>>(RayAttrMask::RayAttrMaskCount)>(
               static_cast<std::underlying_type_t<RayAttrMask>>(attr))
        .to_string();
}

std::vector<std::string> getRayAttrNames() {
#define X(type, name, flagp) #name,
    return std::vector<std::string>{RAYX_X_MACRO_RAY_ATTR};
#undef X
}

RayAttrMask rayAttrStringsToRayAttrMask(const std::vector<std::string>& strings) {
    auto stringToAttr = [](const std::string& str) -> RayAttrMask {
#define X(type, name, flag) \
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

}  // namespace rayx
