#pragma once

#include "Core.h"
#include <vector>

namespace RAYX {


enum class SurfaceCoatingType {
    SubstrateOnly,  // No coating, only substrate
    OneCoating,     // One coating layer
    MultipleCoatings // Multiple coating layers
};

struct RAYX_API Coating{

    struct RAYX_API SubstrateOnly {
        // No additional parameters needed
    };

    struct RAYX_API OneCoating {
        int material;
        double thickness;
        double roughness;
    };

    struct RAYX_API MultilayerCoating {
        int numLayers;
        std::vector<OneCoating> layers;
    };

    SurfaceCoatingType m_type;

    variant::variant<SubstrateOnly, OneCoating, MultilayerCoating> m_coating;

    template<typename T>
    Coating(T t) : m_coating(t) {}
};

} // namespace RAYX