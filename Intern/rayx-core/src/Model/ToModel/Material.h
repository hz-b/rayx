#pragma once

#include "ToModelTrait.h"
#include "Common/Material.h"

namespace rayx::detail {

template <>
struct ToModel<Material> {
    static Material apply(const Material material) { return material; }
};

}  // namespace rayx::detail
