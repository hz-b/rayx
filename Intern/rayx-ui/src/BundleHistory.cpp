#include "BundleHistory.h"

BundleHistory convertRaysToBundleHistory(rayx::Rays rays, const int numSources) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    rays            = rays.sortByPathIdAndPathEventId();
    const auto size = rays.size();

    BundleHistory bundle;

    for (int i = 0; i < size;) {
        const auto current_path_id = rays.path_id[i];
        RayHistory hist;

        while (i < size && rays.path_id[i] == current_path_id) {
            hist.push_back(Ray{
                .m_position    = rays.position(i),
                .m_eventType   = rays.event_type[i],
                .m_direction   = rays.direction(i),
                .m_energy      = rays.energy[i],
                .m_field       = rays.electric_field(i),
                .m_pathLength  = rays.optical_path_length[i],
                .m_order       = rays.order[i],
                .m_lastElement = rays.object_id[i] - numSources,
                .m_sourceID    = rays.source_id[i],
            });

            ++i;
        }

        bundle.push_back(std::move(hist));
    }

    return bundle;
}
