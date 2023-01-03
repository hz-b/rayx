#include "Tracer.h"

#include "Constants.h"
#include "Debug/Debug.h"
#include "Random.h"

namespace RAYX {

const uint64_t BATCH_SIZE = 1000;

std::vector<Ray> Tracer::trace(const Beamline& b) {
    auto rays = b.getInputRays();
    auto randomSeed = randomDouble();
    auto maxSnapshots = (b.m_OpticalElements.size() * 2 + 1);
    auto materialTables = b.calcMinimalMaterialTables();

    // for now we only return the last snapshot.
    std::vector<Ray> outRays;

    for (int batch_id = 0; batch_id * BATCH_SIZE < rays.size(); batch_id++) {
        auto rayIdStart = batch_id * BATCH_SIZE;

        auto remaining_rays = rays.size() - batch_id * BATCH_SIZE;
        auto batch_size = (BATCH_SIZE < remaining_rays) ? BATCH_SIZE : remaining_rays;

        std::vector<Ray> batch;
        for (uint64_t j = 0; j < batch_size; j++) {
            batch.push_back(rays[j + rayIdStart]);
        }

        TraceRawConfig cfg = {
            .m_rays = batch,
            .m_rayIdStart = (double)rayIdStart,
            .m_numRays = (double)rays.size(),
            .m_randomSeed = randomSeed,
            .m_maxSnapshots = (double)maxSnapshots,
            .m_materialTables = materialTables,
            .m_OpticalElements = b.m_OpticalElements,
        };

        auto rawBatchRays = traceRaw(cfg);
        assert(rawBatchRays.size() == batch_size * maxSnapshots);

        for (uint i = 0; i < batch_size; i++) {
            Ray r;
            r.m_weight = W_UNINIT;

            for (uint j = 0; j < maxSnapshots; j++) {
                uint idx = i * maxSnapshots + j;
                if (rawBatchRays[idx].m_weight != W_UNINIT) {
                    r = rawBatchRays[idx];
                }
            }

            if (r.m_weight == W_UNINIT) {
                RAYX_WARN << "returning uninit rays!";
            }

            outRays.push_back(r);
        }
    }

    return outRays;
}

}  // namespace RAYX
