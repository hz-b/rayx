#pragma once

#include <variant>
#include <optional>
#include <array>

#include "Material.h"
#include "Coating.h"

namespace rayx {

struct DetectorBehaviour {};

struct AbsorbBehaviour {};

struct ReflectBehaviour {
    std::optional<Material> substrate = materials::Au;
    std::optional<Coating> coating;
};

struct TransmitBehaviour {} {
    Material substrate = materials::Au;
    bool efficiency = true;
    double substrateThickness = 0.1;
    double substrateRoughness = 0.0;
    std::optional<Coating> coating;
};

struct GratingBehaviour {
    std::array<double, 6> vls = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};      // VLS coefficients
    double lineDensity 0.0;             // lines per mm
    int orderOfDiffraction = 1;         // the diffraction order, usually 1
};

struct RzpBehaviour {
    int imageType = 0;
    int rzpType = 0;
    int derivationMethod = 0;
    int designOrderOfDiffraction = 1;
    int orderOfDiffraction = 1;
    int additionalOrder = 0;
    double designWavelength = 0.0;
    double fresnelZOffset = 0.0;
    double designSagittalEntranceArmLength = 0.0;
    double designSagittalExitArmLength = 0.0;
    double designMeridionalEntranceArmLength = 0.0;
    double designMeridionalExitArmLength = 0.0;
    double designAlphaAngle = 0.0;
    double designBetaAngle = 0.0;
};

struct CrystalBehaviour {
    double dSpacing2 = 0.0;
    double unitCellVolume = 0.0;
    double offsetAngle = 0.0;

    double structureFactorReF0 = 0.0;
    double structureFactorImF0 = 0.0;
    double structureFactorReFH = 0.0;
    double structureFactorImFH = 0.0;
    double structureFactorReFHC = 0.0;
    double structureFactorImFHC = 0.0;
};

using Behaviour = std::variant<DetectorBehaviour, AbsorbBehaviour, ReflectBehaviour, TransmitBehaviour, GratingBehaviour, RzpBehaviour, CrystalBehaviour>;

} // namespace rayx
