#pragma once

#include <vector>

#include "Material.h"

namespace rayx::design {

struct CoatingLayer {
    Material material = Material::Au;
    double thickness  = 0.0;
    double roughness  = 0.0;
};

struct PeriodicCoating {
    std::vector<CoatingLayer> layers = {CoatingLayer()};
    int numPeriods                 = 1;
};

struct Coating {
    PeriodicCoating periodicCoating;

    /// Optional bottom layer (e.g., adhesion layer)
    std::optional<CoatingLayer> bottomLayer;
    /// Optional top layer (e.g., protective layer)
    std::optional<CoatingLayer> topLayer;
};

// TODO: consider implementing depth-graded coatings (distributions: power-law, linear, exponential)

}  // namespace rayx::design
