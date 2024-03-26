#include "Tracer.h"

#include <algorithm>

using uint = unsigned int;

namespace RAYX {

void Tracer::setDevice(int deviceID) { m_deviceID = deviceID; }

/// Get the last event for each ray of the bundle.
std::vector<Ray> extractLastEvents(const BundleHistory& hist) {
    std::vector<Ray> out;
    for (auto& ray_hist : hist) {
        out.push_back(ray_hist.back());
    }

    return out;
}

BundleHistory convertToBundleHistory(const std::vector<Ray>& rays) {
    BundleHistory out;
    for (auto r : rays) {
        out.push_back({r});
    }
    return out;
}

}  // namespace RAYX
