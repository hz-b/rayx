#pragma once

#include <variant>
#include <array>

#include "Coating.h"
#include "Common/Material.h"

namespace rayx::detail::model {

struct DetectBehavior {
    // no parameters
};

struct AbsorbBehavior {
    // no parameters
};

struct IgnoreBehavior {
    // no parameters
};

// struct RectangularDiffractBehavior {
//     double width;
//     double height;
// };
//
// struct EllipticalDiffractBehavior {
//     double horizontalDiameter;
//     double verticalDiameter;
// };

struct ReflectBehavior {
    Material substrate;
    model::Coating coating;
};

struct TransmitBehavior {
    Material substrate;
    double substrateThickness;
    double substrateRoughness;
};

struct GratingBehavior {
    int orderOfDiffraction; /* the diffraction order, usually 1 */
    double lineDensity;     // lines per mm
    std::array<double, 6> vls;          // VLS coefficients
};

struct RzpBehavior {
    int imageType;
    int rzpType;
    int derivationMethod;
    int designOrderOfDiffraction;
    int orderOfDiffraction;
    int additionalOrder;
    double designWavelength;
    double fresnelZOffset;
    double designSagittalEntranceArmLength;
    double designSagittalExitArmLength;
    double designMeridionalEntranceArmLength;
    double designMeridionalExitArmLength;
    double designAlphaAngle;
    double designBetaAngle;
};

struct CrystalBehavior {
    double dSpacing2;
    double unitCellVolume;
    double offsetAngle;
    double structureFactorReF0;
    double structureFactorImF0;
    double structureFactorReFH;
    double structureFactorImFH;
    double structureFactorReFHC;
    double structureFactorImFHC;
};

using Behavior =
    std::variant<DetectBehavior, AbsorbBehavior, IgnoreBehavior, ReflectBehavior, TransmitBehavior, GratingBehavior, RzpBehavior, CrystalBehavior>;

}  // namespace rayx::detail::model
