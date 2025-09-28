#pragma once

#include "Core.h"
#include <vector>

namespace RAYX {

#undef m_private_serialization_params


enum class SurfaceCoatingType {
    SubstrateOnly,  // No coating, only substrate
    OneCoating,     // One coating layer
    MultipleCoatings // Multiple coating layers
};

struct Layer {
    int material;
    double thickness;
    double roughness;
    
};

struct MultilayerCoating {
    int numLayers;
    std::vector<Layer> layers;
};

struct Coating {
    // the type of this behaviour, see the BTYPE constants.
    // the type describes how the m_private_serialization_params need to be interpreted.
    SurfaceCoatingType m_type;
    double m_private_serialization_params[32];
};

// substrate only
RAYX_FN_ACC
inline Coating serializeSubstrateOnly() {
    Coating c;
    c.m_type = SurfaceCoatingType::SubstrateOnly;
    return c;
}

//////////////////////////////
// OneCoating
//////////////////////////////

struct OneCoating
{
    int material; // Material ID of the coating
    double thickness;
    double roughness;
};

RAYX_FN_ACC
inline Coating serializeOneCoating(const OneCoating oc) {
    Coating c;
    c.m_type = SurfaceCoatingType::OneCoating;
    c.m_private_serialization_params[0] = static_cast<double>(oc.material);
    c.m_private_serialization_params[1] = oc.thickness;
    c.m_private_serialization_params[2] = oc.roughness;
    return c;
}

RAYX_FN_ACC
inline OneCoating deserializeOneCoating(const Coating c) {
    OneCoating oc;
    oc.material = static_cast<int>(c.m_private_serialization_params[0]);
    oc.thickness = c.m_private_serialization_params[1];
    oc.roughness = c.m_private_serialization_params[2];
    return oc;
}

///////////////
// MultilayerCoating
///////////////

RAYX_FN_ACC
inline Coating serializeMultilayer(MultilayerCoating layers) {
    Coating c;
    c.m_type = SurfaceCoatingType::MultipleCoatings;
    c.m_private_serialization_params[0] = static_cast<double>(layers.numLayers);
    for (size_t i = 0; i < layers.layers.size(); ++i) {
        c.m_private_serialization_params[1 + i * 3] = static_cast<double>(layers.layers[i].material);
        c.m_private_serialization_params[2 + i * 3] = layers.layers[i].thickness;
        c.m_private_serialization_params[3 + i * 3] = layers.layers[i].roughness;
    }
    return c;
}

RAYX_FN_ACC
inline MultilayerCoating deserializeMultilayer(const Coating c) {
    MultilayerCoating layers;
    layers.numLayers = static_cast<int>(c.m_private_serialization_params[0]);
    layers.layers.resize(layers.numLayers);
    for (int i = 0; i < layers.numLayers; ++i) {
        layers.layers[i].material = static_cast<int>(c.m_private_serialization_params[1 + i * 3]);
        layers.layers[i].thickness = c.m_private_serialization_params[2 + i * 3];
        layers.layers[i].roughness = c.m_private_serialization_params[3 + i * 3];
    }
    return layers;
}

} // namespace RAYX