#include "Coating.h"

namespace rayx::detail {

model::Coating toModel(const std::optional<Coating>& optCoating) {
    if (!optCoating) {
        return model::Coating{
            .numLayers   = 0,
            .materials   = {},
            .thicknesses = {},
            .roughnesses = {},
        };
    }

    const auto& coating = *optCoating;

    auto materials   = std::vector<Material>{};
    auto thicknesses = std::vector<double>{};
    auto roughnesses = std::vector<double>{};

    if (coating.bottomLayer()) {
        materials.push_back(coating.bottomLayer()->material());
        thicknesses.push_back(coating.bottomLayer()->thickness());
        roughnesses.push_back(coating.bottomLayer()->roughness());
    }

    for (int i = 0; i < coating.numPeriods(); ++i) {
        for (const auto& layer : coating.layers()) {
            materials.push_back(layer.material());
            thicknesses.push_back(layer.thickness());
            roughnesses.push_back(layer.roughness());
        }
    }

    if (coating.topLayer()) {
        materials.push_back(coating.topLayer()->material());
        thicknesses.push_back(coating.topLayer()->thickness());
        roughnesses.push_back(coating.topLayer()->roughness());
    }

    assert(materials.size() == thicknesses.size() && materials.size() == roughnesses.size());

    return model::Coating{
        .numLayers   = static_cast<int>(materials.size()),
        .materials   = materials,
        .thicknesses = thicknesses,
        .roughnesses = roughnesses,
    };
}

}  // namespace rayx::detail
