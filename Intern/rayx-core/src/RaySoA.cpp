#include "RaySoA.h"

#include "Debug/Instrumentor.h"

namespace RAYX {

RaySoA bundleHistoryToRaySoA(const BundleHistory& bundle) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    RaySoA rays;

    int32_t event_id = 0;
    int32_t path_id = 0;
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
    rays.num_paths = path_id;
    return rays;
}

BundleHistory raySoAToBundleHistory(const RaySoA& rays) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto bundle = BundleHistory(rays.num_paths);

    for (int i = 0; i < rays.num_events; ++i) {
        const auto path_id = rays.path_id[i];
        bundle[path_id].push_back(rays.ray(i));
    }

    return bundle;
}

}  // namespace RAYX
