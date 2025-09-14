#include "Rays.h"

#include "Debug/Instrumentor.h"

namespace RAYX {

Rays bundleHistoryToRays(const BundleHistory& bundle) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    Rays rays;

    int32_t event_id = 0;
    int32_t path_id  = 0;
    for (const auto& hist : bundle) {
        for (const auto& event : hist) {
            rays.path_id.push_back(path_id);

#define X(type, name, flag, map) rays.name.push_back(event.map);

            RAYX_X_MACRO_RAY_ATTR_MAPPED
#undef X

            ++event_id;
        }
        ++path_id;
    }

    rays.num_events = event_id;
    rays.num_paths  = path_id;
    return rays;
}

BundleHistory raySoAToBundleHistory(const Rays& rays) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (rays.num_events == 0) return BundleHistory();
    int maxPathId = *std::max_element(rays.path_id.begin(), rays.path_id.end());
    auto bundle   = BundleHistory(maxPathId + 1);

    for (int i = 0; i < rays.num_events; ++i) {
        const auto path_id = rays.path_id[i];
        bundle[path_id].push_back(rays.ray(i));
    }

    return bundle;
}

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
