#include "Tracer.h"

#include <algorithm>

#include "Beamline/OpticalElement.h"
#include "RAY-Core.h"
#include "Random.h"
#include "Shader/Constants.h"

using uint = unsigned int;

namespace RAYX {

BundleHistory Tracer::trace(const Beamline& b, Sequential seq, uint64_t max_batch_size, int thread_count, unsigned int maxEvents, int startEventID) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "maxEvents: " << maxEvents;

    auto rays = b.getInputRays(thread_count);

    // don't trace if there are no optical elements
    if (b.m_OpticalElements.size() == 0) {
        // an empty history suffices, nothing is happening to the rays!
        BundleHistory result;
        return result;
    }

    auto randomSeed = randomDouble();
    auto materialTables = b.calcMinimalMaterialTables();

    // This will be the complete BundleHistory.
    // All initialized events will have been put into this by the end of this function.
    BundleHistory result;

    // iterate over all batches.
    for (int batch_id = 0; batch_id * max_batch_size < rays.size(); batch_id++) {
        // `rayIdStart` is the ray-id of the first ray of this batch.
        // All previous batches consisted of `max_batch_size`-many rays.
        // (Only the last batch might be smaller than max_batch_size, if the number of rays isn't divisible by max_batch_size).
        auto rayIdStart = batch_id * max_batch_size;

        auto remaining_rays = rays.size() - batch_id * max_batch_size;

        // The number of input-rays that we put into this batch.
        // Typically equal to max_batch_size, except for the last batch.
        auto batch_size = (max_batch_size < remaining_rays) ? max_batch_size : remaining_rays;

        std::vector<Element> elements;
        elements.reserve(b.m_OpticalElements.size());
        for (const auto& e : b.m_OpticalElements) {
            elements.push_back(e.m_element);
        }

        // create a TraceRawConfig.
        TraceRawConfig cfg = {
            .m_rays = std::vector<Ray>(rays.begin() + rayIdStart, rays.begin() + rayIdStart + batch_size),
            .m_rayIdStart = (double)rayIdStart,
            .m_numRays = (double)rays.size(),
            .m_randomSeed = randomSeed,
            .m_maxEvents = (double)maxEvents,
            .m_startEventID = (double)startEventID,
            .m_materialTables = materialTables,
            .m_elements = elements,
        };

        auto sequential = (double)(seq == Sequential::Yes);
        PushConstants pushConstants = {.rayIdStart = (double)rayIdStart,
                                       .numRays = (double)rays.size(),
                                       .randomSeed = randomSeed,
                                       .maxEvents = (double)maxEvents,
                                       .sequential = sequential,
                                       .startEventID = (double)startEventID};
        setPushConstants(&pushConstants);

        // run the actual tracer (GPU/CPU).
        std::vector<Ray> rawBatch;
        {
            RAYX_PROFILE_SCOPE_STDOUT("Tracing");
            rawBatch = traceRaw(cfg);
            RAYX_LOG << "Traced " << rawBatch.size() << " events.";
            assert(rawBatch.size() == batch_size * (maxEvents - startEventID));
        }

        // put all events from the rawBatch to unified `BundleHistory result`.
        {
            RAYX_PROFILE_SCOPE_STDOUT("BundleHistory-calculation");
            for (uint i = 0; i < batch_size; i++) {
                // We now create the Rayhistory for the `i`th ray of the batch:
                RayHistory hist;
                hist.reserve(maxEvents - startEventID);

                // iterate through the event-indices that were allocated for this particular ray.
                for (uint j = 0; j < maxEvents - startEventID; j++) {
                    // each ray has space for `maxEvents - startEventID` many events.
                    // Hence the first event for ray i is always at index `i * (maxEvents - startEventID)`.
                    uint idx = i * (maxEvents - startEventID) + j;
                    Ray r = rawBatch[idx];

                    // we discard UNINIT events.
                    // These events only come up, if the ray had less than `maxEvents`-many events.
                    if (r.m_eventType == ETYPE_UNINIT) {
                        // further we know that after one UNINIT event, there will only come further UNINIT events.
                        // Hence we are done for this ray, and will look for the next one.
                        break;
                    } else {
                        hist.push_back(r);
                    }
                }

                // We put the `hist` for the `i`th ray of the batch into the global `BundleHistory result`.
                result.push_back(hist);
            }
        }
    }

    return result;
}
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
