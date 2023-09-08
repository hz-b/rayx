#include "Tracer.h"

#include <algorithm>

#include "Beamline/OpticalElement.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "Shared/Constants.h"

using uint = unsigned int;

namespace RAYX {

BundleHistory Tracer::trace(const Beamline& b, uint64_t max_batch_size) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rays = b.getInputRays();
    auto randomSeed = randomDouble();
    auto maxEvents = b.m_OpticalElements.size() + 2;
    auto materialTables = b.calcMinimalMaterialTables();

    BundleHistory result;

    for (int batch_id = 0; batch_id * max_batch_size < rays.size(); batch_id++) {
        auto rayIdStart = batch_id * max_batch_size;

        auto remaining_rays = rays.size() - batch_id * max_batch_size;
        auto batch_size = (max_batch_size < remaining_rays) ? max_batch_size : remaining_rays;

        std::vector<Ray> batch;
        batch.reserve(batch_size);
        std::copy(rays.begin() + rayIdStart, rays.begin() + rayIdStart + batch_size, std::back_inserter(batch));

        std::vector<Element> elements;
        for (auto e : b.m_OpticalElements) {
            elements.push_back(e.m_element);
        }

        TraceRawConfig cfg = {
            .m_rays = batch,
            .m_rayIdStart = (double)rayIdStart,
            .m_numRays = (double)rays.size(),
            .m_randomSeed = randomSeed,
            .m_maxEvents = (double)maxEvents,
            .m_materialTables = materialTables,
            .m_elements = elements,
        };

        PushConstants* pushConstants = new PushConstants{
            .rayIdStart = (double)rayIdStart, .numRays = (double)rays.size(), .randomSeed = randomSeed, .maxEvents = (double)maxEvents};
        setPushConstants(pushConstants);

        BundleHistory rawBatchHistory;
        {
            RAYX_PROFILE_SCOPE_STDOUT("Tracing");
            rawBatchHistory = traceRaw(cfg);
            for ([[maybe_unused]] const auto& _events : rawBatchHistory) {  // Sanity Check
                assert(batch_size == _events.size());
            }
        }

        {
            RAYX_PROFILE_SCOPE_STDOUT("Snapshotting");
            auto maxSnapshots = rawBatchHistory.size();
            //std::cout << "max snap:" << maxSnapshots << std::endl;
            result.reserve(batch_size);

            for (uint i = 0; i < batch_size; i++) {
                RayHistory snapshots;
                snapshots.reserve(maxSnapshots);
                for (uint j = 0; j < maxSnapshots; j++) {
                    Ray r = rawBatchHistory[j][i];
                    //std::cout << r.m_eventType << std::endl;
                    if (r.m_eventType != ETYPE_UNINIT) {
                        snapshots.push_back(r);
                    }
                }
                snapshots.shrink_to_fit();
                //std::cout << "snap size: " << snapshots.size() << std::endl;
                result.push_back(snapshots);
            }
        }
    }
    std::cout << result.size() << std::endl;
    return result;
}
void Tracer::setDevice(int deviceID) { m_deviceID = deviceID; }

/// Get the last event for each ray of the bundle.
std::vector<Event> extractLastEvents(const BundleHistory& hist) {
    std::vector<Event> out;
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
