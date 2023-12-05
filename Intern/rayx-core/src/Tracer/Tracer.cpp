#include "Tracer.h"

#include <algorithm>

#include "Beamline/OpticalElement.h"
#include "RAY-Core.h"
#include "Random.h"
#include "Shader/Constants.h"

using uint = unsigned int;

namespace RAYX {

BundleHistory Tracer::trace(const Beamline& b, Sequential seq, uint64_t max_batch_size, int thread_count) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rays = b.getInputRays(thread_count);

    // don't trace if there are no optical elements
    if (b.m_OpticalElements.size() == 0) {
        BundleHistory result;
        for (auto r : rays) {
            auto ray = r;
            ray.m_eventType = ETYPE_FLY_OFF;
            result.push_back({ray});
        }
        return result;
    }

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
        elements.reserve(b.m_OpticalElements.size());
        for (const auto& e : b.m_OpticalElements) {
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

        auto sequential = (double)(seq == Sequential::Yes);
        PushConstants pushConstants = {.rayIdStart = (double)rayIdStart,
                                       .numRays = (double)rays.size(),
                                       .randomSeed = randomSeed,
                                       .maxEvents = (double)maxEvents,
                                       .sequential = sequential};
        setPushConstants(&pushConstants);

        RayHistory rawBatchHistory;
        {
            RAYX_PROFILE_SCOPE_STDOUT("Tracing");
            rawBatchHistory = traceRaw(cfg);
            assert(rawBatchHistory.size() == batch_size * maxEvents);
        }

        {
            RAYX_PROFILE_SCOPE_STDOUT("BundleHistory-calculation");
            for (uint i = 0; i < batch_size; i++) {
                RayHistory hist;
                hist.reserve(maxEvents);
                for (uint j = 0; j < maxEvents; j++) {
                    uint idx = i * maxEvents + j;
                    Ray r = rawBatchHistory[idx];
                    if (r.m_eventType != ETYPE_UNINIT) {
                        hist.push_back(r);
                    }
                }
                result.push_back(hist);
            }
        }
    }

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
