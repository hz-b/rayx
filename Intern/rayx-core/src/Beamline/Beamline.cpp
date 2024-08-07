#include "Beamline.h"

#include <array>

#include "Debug/Instrumentor.h"

namespace RAYX {

Beamline::Beamline() = default;
Beamline::~Beamline() = default;

MaterialTables Beamline::calcMinimalMaterialTables() const {
    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);

    for (const auto& e : m_DesignElements) {
        int material = static_cast<int>(e.getMaterial());  // in [1, 92]
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

}  // namespace RAYX
