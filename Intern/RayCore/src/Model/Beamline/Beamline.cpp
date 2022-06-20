#include "Beamline.h"

#include <iostream>

#include "Debug/Instrumentor.h"
#include "Model/Beamline/Objects/Objects.h"

namespace RAYX {
Beamline::Beamline() {}

Beamline::~Beamline() {}

RayList Beamline::getInputRays() const {
    RayList list;

    list.insertVector(m_extraRays);

    for (auto s : m_LightSources) {
        auto sub = s->getRays();
        list.insertVector(sub);
    }

    return list;
}

MaterialTables Beamline::calcMinimalMaterialTables() const {
    std::array<bool, 92> relevantMaterials;
    relevantMaterials.fill(false);

    for (auto e : m_OpticalElements) {
        int material = e->getSurfaceParams()[14];  // in [1, 92]
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

}  // namespace RAYX
