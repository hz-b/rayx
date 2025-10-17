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
        int material[1000];
        double thickness[1000];
        double roughness[1000];
    };

    variant::variant<SubstrateOnly, OneCoating, MultilayerCoating> m_coating;

    Coating() : Coating(SubstrateOnly{}) {}

    template<typename T>
    Coating(T t) : m_coating(t) {}

    template <typename T>
    bool is() const {
        return variant::holds_alternative<T>(m_coating);
    }
};

} // namespace RAYX