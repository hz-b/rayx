#pragma once

#include "ToModelTrait.h"
#include "rayx/Common/Enums.h"

namespace rayx::detail {

////////////////////////////////////////////////////////////
// Source
////////////////////////////////////////////////////////////

template <>
struct ToModel<UndulatorSigmaType> {
    static UndulatorSigmaType apply(const UndulatorSigmaType type) { return type; }
};

template <>
struct ToModel<ElectronEnergyOrientation> {
    static ElectronEnergyOrientation apply(const ElectronEnergyOrientation orientation) { return orientation; }
};

////////////////////////////////////////////////////////////
// Element
////////////////////////////////////////////////////////////

template <>
struct ToModel<BackFaceBehavior> {
    static BackFaceBehavior apply(const BackFaceBehavior behavior) { return behavior; }
};

////////////////////////////////////////////////////////////
// Curvature
////////////////////////////////////////////////////////////

template <>
struct ToModel<ToroidType> {
    static ToroidType apply(const ToroidType type) { return type; }
};

}  // namespace rayx::detail
