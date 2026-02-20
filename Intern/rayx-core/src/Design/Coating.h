#pragma once

#include <vector>

#include "Material.h"

namespace rayx {

// TODO: depending on the tracer implementation, the maximum numbe of layers may be limited. then a validation should be added

struct CoatingLayer {
    constexpr CoatingLayer(Material material, double thickness) {
        this->material(material);
        this->thickness(thickness);
    }

    RAYX_PROPERTY(CoatingLayer, Material, material);
    RAYX_VALIDATED_PROPERTY(CoatingLayer, double, thickness, detail::validateGreaterZero);
    RAYX_VALIDATED_PROPERTY(CoatingLayer, double, roughness, detail::validateGreaterEqualZero) = 0.0;
};

struct PeriodicCoating {
    constexpr PeriodicCoating(std::vector<CoatingLayer> layers) { this->layers(layers); }

    RAYX_VALIDATED_PROPERTY(PeriodicCoating, std::vector<CoatingLayer>, layers, detail::validateVectorSizeGreaterZero);
    RAYX_VALIDATED_PROPERTY(PeriodicCoating, int, numPeriods, detail::validateGreaterZero) = 1;

    // optional bottom layer (e.g., adhesion layer)
    RAYX_NESTED_PROPERTY(PeriodicCoating, std::optional<CoatingLayer>, bottomLayer);
    // optional top layer (e.g., protective layer)
    RAYX_NESTED_PROPERTY(PeriodicCoating, std::optional<CoatingLayer>, topLayer);
};

using Coating = std::variant<PeriodicCoating>;
// TODO: consider implementing depth-graded coatings (distributions: power-law, linear, exponential)

}  // namespace rayx
