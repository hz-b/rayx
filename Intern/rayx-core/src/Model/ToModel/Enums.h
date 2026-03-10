#pragma once

#include "Common/Enums.h"
#include "ToModelTrait.h"

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

template <>
struct ToModel<CylinderDirection> {
    static CylinderDirection apply(const CylinderDirection direction) { return direction; }
};

template <>
struct ToModel<ParabolicCurvatureType> {
    static ParabolicCurvatureType apply(const ParabolicCurvatureType type) { return type; }
};

template <>
struct ToModel<FigureRotation> {
    static FigureRotation apply(const FigureRotation rotation) { return rotation; }
};

}  // namespace rayx::detail
