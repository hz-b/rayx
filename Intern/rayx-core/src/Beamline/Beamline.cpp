#include "Beamline.h"

#include <array>

#include "Debug/Instrumentor.h"

namespace RAYX {
Beamline::Beamline() = default;
Beamline::~Beamline() = default;

std::vector<Ray> Beamline::getInputRays(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (m_LightSources.size() == 0) {
        return {};
    }

    // count number of rays.
    uint32_t raycount = 0;
    for (const auto& s : m_LightSources) {
        raycount += s->m_numberOfRays;
    }

    // We add all remaining rays into the rays of the first light source.
    // This is efficient because in most cases there is just one light source, and hence copying them again is unnecessary.
    std::vector<Ray> list = m_LightSources[0]->getRays();

    if (m_LightSources.size() > 1) {
        list.reserve(raycount);

    for (const auto& s : m_LightSources) {
        std::vector<Ray> sub;

        sub = s->getRays(thread_count);

        list.insert(list.end(), sub.begin(), sub.end());
    }

    return list;
}

MaterialTables Beamline::calcMinimalMaterialTables() const {
    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);

    for (const auto& e : m_OpticalElements) {
        int material = (int)e.m_element.m_material;  // in [1, 92]
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

}  // namespace RAYX
