#pragma once

#include "Core.h"


namespace RAYX {

#undef m_private_serialization_params


enum class SurfaceCoatingType {
    SubstrateOnly,  // No coating, only substrate
    OneCoating,     // One coating layer
    MultipleCoatings // Multiple coating layers
};

struct Coating {
    // the type of this behaviour, see the BTYPE constants.
    // the type describes how the m_private_serialization_params need to be interpreted.
    SurfaceCoatingType m_type;

    // These params are private. use the serialize & deserialize functions below instead.
    double m_private_serialization_params[16];
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
    c.m_private_serialization_params[0] = oc.material;
    c.m_private_serialization_params[1] = oc.thickness;
    c.m_private_serialization_params[2] = oc.roughness;
    return c;
}
inline OneCoating deserializeOneCoating(const Coating c) {
    OneCoating oc;
    oc.material = c.m_private_serialization_params[0];
    oc.thickness = c.m_private_serialization_params[1];
    oc.roughness = c.m_private_serialization_params[2];
    return oc;
}

} // namespace RAYX

