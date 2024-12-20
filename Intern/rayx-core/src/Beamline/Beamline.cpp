#include "Beamline.h"

#include <array>

#include "Debug/Instrumentor.h"

namespace RAYX {

std::vector<Ray> Beamline::getInputRays(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<DesignSource> sources = m_RootGroup.getAllSources();

    if (sources.size() == 0) {
        return {};
    }

    // count number of rays.
    uint32_t raycount = 0;

    for (DesignSource dSource : sources) {
        raycount += (uint32_t)dSource.getNumberOfRays();
    }

    // We add all remaining rays into the rays of the first light source.
    // This is efficient because in most cases there is just one light source, and hence copying them again is unnecessary.
    std::vector<Ray> list = sources[0].compile(thread_count);
    for (Ray& r : list) {
        r.m_sourceID = 0;  // the first light source has ID 0.
    }

    if (sources.size() > 1) {
        list.reserve(raycount);

        for (size_t i = 1; i < sources.size(); i++) {
            std::vector<Ray> sub = sources[i].compile(thread_count);
            for (Ray& r : sub) {
                r.m_sourceID = static_cast<double>(i);
            }
            list.insert(list.end(), sub.begin(), sub.end());
        }
    }
    return list;
}

MaterialTables Beamline::calcMinimalMaterialTables() const {
    std::vector<DesignElement> elements = m_RootGroup.getAllElements();

    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);

    for (const auto& e : elements) {
        int material = static_cast<int>(e.getMaterial());  // in [1, 92]
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

void Beamline::addNodeToRoot(BeamlineNode&& node) {
    // Add to the root group
    m_RootGroup.addChild(std::move(node));
}

}  // namespace RAYX
