#include "Tracer.h"

#include "Constants.h"
#include "Debug/Debug.h"
#include "Random.h"

namespace RAYX {

std::vector<Ray> Tracer::trace(const Beamline& b) {
    auto rays = b.getInputRays();
    auto randomSeed = randomDouble();
    auto maxSnapshots = (b.m_OpticalElements.size() * 2 + 1);
    auto materialTables = b.calcMinimalMaterialTables();

    TraceRawConfig cfg = {
        .m_rays = rays,
        .m_rayIdStart = 0,
        .m_numRays = (double)rays.size(),
        .m_randomSeed = randomSeed,
        .m_maxSnapshots = (double)maxSnapshots,
        .m_materialTables = materialTables,
        .m_OpticalElements = b.m_OpticalElements,
    };

    auto rawRays = traceRaw(cfg);
    assert(rawRays.size() == rays.size() * maxSnapshots);

    // for now we only return the last snapshot.
    std::vector<Ray> outRays;
    for (uint i = 0; i < rays.size(); i++) {
        Ray r;
        r.m_weight = W_UNINIT;

        for (uint j = 0; j < maxSnapshots; j++) {
            uint idx = i * maxSnapshots + j;
            if (rawRays[idx].m_weight != W_UNINIT) {
                r = rawRays[idx];
            }
        }

        if (r.m_weight == W_UNINIT) {
            RAYX_WARN << "returning uninit rays!";
        }

        outRays.push_back(r);
    }

    return outRays;
}

}  // namespace RAYX
