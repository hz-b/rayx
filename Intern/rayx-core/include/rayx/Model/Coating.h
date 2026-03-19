#pragma once

#include <vector>

#include "rayx/Common/Material.h"

namespace rayx::detail::model {

struct Coating {
    int numLayers;
    std::vector<Material> materials;
    std::vector<double> thicknesses;
    std::vector<double> roughnesses;
};

}  // namespace rayx::detail::model
