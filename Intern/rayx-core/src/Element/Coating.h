#pragma once

#include <vector>

#include "Core.h"
#include "Variant.h"

namespace rayx {

enum class SurfaceCoatingType {
    SubstrateOnly,    // No coating, only substrate
    OneCoating,       // One coating layer
    MultipleCoatings  // Multiple coating layers
};

namespace detail {
struct CoatingTypes {
    struct RAYX_API SubstrateOnly{
        // No additional parameters needed
    };

    struct RAYX_API OneCoating {
        int material;
        double thickness;
        double roughness;
    };

    struct RAYX_API MultilayerCoating {
        int numLayers;
        int material[1000];
        double thickness[1000];
        double roughness[1000];
    };
};
}  // namespace detail

using Coating =
    Variant<detail::CoatingTypes, detail::CoatingTypes::SubstrateOnly, detail::CoatingTypes::OneCoating, detail::CoatingTypes::MultilayerCoating>;

}  // namespace rayx
