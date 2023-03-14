#include "Tracer.h"

#include <algorithm>

#include "Constants.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Model/Beamline/OpticalElement.h"
#include "Random.h"

using uint = unsigned int;

namespace RAYX {

const uint64_t BATCH_SIZE = 100000;

Rays Tracer::trace(const Beamline& b) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rays = b.getInputRays();
    auto randomSeed = randomDouble();
    auto maxSnapshots = b.m_OpticalElements.size() + 2;
    auto materialTables = b.calcMinimalMaterialTables();

    Rays result;

    for (int batch_id = 0; batch_id * BATCH_SIZE < rays.size(); batch_id++) {
        auto rayIdStart = batch_id * BATCH_SIZE;

        auto remaining_rays = rays.size() - batch_id * BATCH_SIZE;
        auto batch_size = (BATCH_SIZE < remaining_rays) ? BATCH_SIZE : remaining_rays;

        std::vector<Ray> batch;
        batch.reserve(batch_size);
        std::copy(rays.begin() + rayIdStart, rays.begin() + rayIdStart + batch_size, std::back_inserter(batch));

        std::vector<Element> elements;
        for (auto e : b.m_OpticalElements) {
            elements.push_back(e->intoElement());
        }

        TraceRawConfig cfg = {
            .m_rays = batch,
            .m_rayIdStart = (double)rayIdStart,
            .m_numRays = (double)rays.size(),
            .m_randomSeed = randomSeed,
            .m_maxSnapshots = (double)maxSnapshots,
            .m_materialTables = materialTables,
            .m_elements = elements,
        };

        PushConstants pushConsants = {
            .rayIdStart = (double)rayIdStart, .numRays = (double)rays.size(), .randomSeed = randomSeed, .maxSnapshots = (double)maxSnapshots};
        setPushConstants(&pushConsants);

        RAYX::Snapshots rawBatchRays;
        {
            RAYX_PROFILE_SCOPE_STDOUT("Tracing");
            rawBatchRays = traceRaw(cfg);
            assert(rawBatchRays.size() == batch_size * maxSnapshots);
        }

        {
            RAYX_PROFILE_SCOPE_STDOUT("Snapshoting");
            for (uint i = 0; i < batch_size; i++) {
                Snapshots snapshots;
                snapshots.reserve(maxSnapshots);
                for (uint j = 0; j < maxSnapshots; j++) {
                    uint idx = i * maxSnapshots + j;
                    Ray r = rawBatchRays[idx];
                    if (r.m_weight != W_UNINIT) {
                        snapshots.push_back(r);
                    }
                }
                result.push_back(snapshots);
            }
        }
    }

    return result;
}
/**
 * @brief Get Rays in last snapshot
 *
 * @param rays
 * @return std::vector<Ray>
 */
std::vector<Ray> extractLastSnapshot(const Rays& rays) {
    std::vector<Ray> out;
    for (auto& snapshots : rays) {
        out.push_back(snapshots.back());
    }

    return out;
}
/**
 * @brief Get Rays in first snapshot
 *
 * @param rays
 * @return std::vector<Ray>
 */
std::vector<Ray> extracFirstSnapshot(const Rays& rays) {
    std::vector<Ray> out;
    for (auto& snapshots : rays) {
        out.push_back(snapshots.front());
    }

    return out;
}
/***
 * Get Rays Nth snapshot
 */
std::vector<Ray> extracNthSnapshot(const Rays& rays, int snapshotID) {
    std::vector<Ray> out;
    int skipped = 0;
    for (auto& snapshots : rays) {
        if ((int)snapshots.size() - 1 >= snapshotID) {
            out.push_back(snapshots[snapshotID]);
        } else {
            skipped++;
        }
    }
    RAYX_VERB << "Skipped " << skipped << " snapshots while extracting.";
    return out;
}

Rays convertToRays(const std::vector<Ray>& rays) {
    Rays out;
    for (auto r : rays) {
        out.push_back({r});
    }
    return out;
}

}  // namespace RAYX
