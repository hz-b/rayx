#include "Beamline.h"

#include <array>

#include "Debug/Instrumentor.h"

namespace RAYX {
Beamline::Beamline() = default;
Beamline::~Beamline() = default;

std::vector<Ray> Beamline::getInputRays(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (m_DesignSources.size() == 0) {
        return {};
    }

    // count number of rays.
    uint32_t raycount = 0;

    for (DesignSource lsPtr : m_DesignSources) {
        raycount += lsPtr.getNumberOfRays();
    }

    // We add all remaining rays into the rays of the first light source.
    // This is efficient because in most cases there is just one light source, and hence copying them again is unnecessary.
    std::vector<Ray> list = m_DesignSources[0].compile(thread_count);
    for (Ray& r : list) {
        r.m_sourceID = 0;  // the first light source has ID 0.
    }

    if (m_DesignSources.size() > 1) {
        list.reserve(raycount);

        for (unsigned int i = 1; i < m_DesignSources.size(); i++) {
            std::vector<Ray> sub = m_DesignSources[i].compile(thread_count);
            for (Ray& r : sub) {
                r.m_sourceID = i;
            }
            list.insert(list.end(), sub.begin(), sub.end());
        }
    }
    return list;
}

MaterialTables Beamline::calcMinimalMaterialTables() const {
    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);

    for (const auto& e : m_DesignElements) {
        int material = static_cast<int>(e.getMaterial());  // in [1, 92]
        // TODO material = "extract the chosen material from `e`".
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

}  // namespace RAYX
