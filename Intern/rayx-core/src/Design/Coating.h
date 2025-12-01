#pragma once

#include <vector>

#include "Material.h"

namespace rayx {

struct SingleLayerCoating {
    Material material = Material::Au;
    double thickness = 0.0;
    double roughness = 0.0;
};

struct RepeatedCoating {
    std::vector<SingleLayerCoating> layers = { SingleLayerCoating() };
    int numInstances = 1;
};

using Coating = std::vector<RepeatedCoating>;

} // namespace rayx
