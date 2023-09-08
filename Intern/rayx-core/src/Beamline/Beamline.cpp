#include "Beamline.h"

#include <array>

#include "Beamline/Objects/Objects.h"
#include "Debug/Instrumentor.h"

namespace RAYX {
Beamline::Beamline() = default;
Beamline::~Beamline() = default;

std::vector<Ray> Beamline::getInputRays(int THREAD_COUNT) const {
    RAYX_PROFILE_FUNCTION();
    std::vector<Ray> list;
    uint32_t raycount = 0;
    for (const auto& s : m_LightSources) {
        raycount += s->m_numberOfRays;
    }
    list.reserve(raycount);

    for (const auto& s : m_LightSources) {
        std::vector<Ray> sub;
        if(THREAD_COUNT == 0){
            sub = s->getRays();
        }else{
            sub = s->getRays(THREAD_COUNT);
        }
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
