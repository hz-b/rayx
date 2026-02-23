#pragma once

#include "Design/Coating.h"
#include "ToModel.h"

namespace rayx::detail::model {

struct Coating {
    int numLayers;
    std::vector<Material> materials;
    std::vector<double> thicknesses;
    std::vector<double> roughnesses;
};

}  // namespace rayx::detail::model

namespace rayx::detail {

model::Coating toModel(const std::optional<Coating>& optCoating);

}
