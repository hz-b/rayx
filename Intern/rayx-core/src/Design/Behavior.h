#pragma once

#include <array>
#include <optional>
#include <variant>

#include "Coating.h"
#include "Material.h"

namespace rayx {

/// Behavior of element when a ray hits the back side of the element (i.e., the side opposite to the normal direction).
enum class BackFaceBehavior {
    Ignore,
    Absorb,
};

/// Ray does not interact with the element, but records `HitElement` event.
struct DetectBehavior {};

/// Ray gets absorbed and records `AbsorbedFrontFace` event.
struct AbsorbBehavior {};

// TODO: this is essentially useless, because its the same as if the element was not there at all.
/// Ray does not interact with the element and records no events.
struct IgnoreBehavior {};

struct ReflectBehavior {
    constexpr ReflectBehavior(Material substrate) { this->substrate(substrate); }

    RAYX_PROPERTY(ReflectBehavior, Material, substrate);
    // RAYX_PROPERTY(ReflectBehavior, double, substrateRoughness) = 0.0; // TODO: currently not supported
    RAYX_NESTED_PROPERTY(ReflectBehavior, std::optional<Coating>, coating);
};

struct TransmitBehavior {
    constexpr TransmitBehavior(Material substrate, double substrateThickness) {
        this->substrate(substrate);
        this->substrateThickness(substrateThickness);
    }

    RAYX_PROPERTY(TransmitBehavior, Material, substrate);
    RAYX_VALIDATED_PROPERTY(TransmitBehavior, double, substrateThickness, detail::validateGreaterZero);
    RAYX_VALIDATED_PROPERTY(TransmitBehavior, double, substrateRoughness, detail::validateGreaterEqualZero) = 0.0;
    // TODO: implement multilayer foil
    // RAYX_NESTED_PROPERTY(TransmitBehavior, std::optional<Coating>, coating);
};

struct GratingBehavior {
    constexpr GratingBehavior(double lineDensity) { this->lineDensity(lineDensity); }

    RAYX_VALIDATED_PROPERTY(GratingBehavior, double, lineDensity, detail::validateGreaterZero);               // lines per mm
    RAYX_PROPERTY(GratingBehavior, int, orderOfDiffraction)              = 1;                                 // the diffraction order, usually 1
    RAYX_PROPERTY(GratingBehavior, std::array<double RAYX_COMMA 6>, vls) = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };  // VLS coefficients
};

struct RzpBehavior {
    // TODO: find out which properties need validation
    // TODO: find out which properties can use more specific types (e.g., enums, units)
    RAYX_PROPERTY(RzpBehavior, int, imageType)                            = 0;
    RAYX_PROPERTY(RzpBehavior, int, rzpType)                              = 0;
    RAYX_PROPERTY(RzpBehavior, int, derivationMethod)                     = 0;
    RAYX_PROPERTY(RzpBehavior, int, designOrderOfDiffraction)             = 1;
    RAYX_PROPERTY(RzpBehavior, int, orderOfDiffraction)                   = 1;
    RAYX_PROPERTY(RzpBehavior, int, additionalOrder)                      = 0;
    RAYX_PROPERTY(RzpBehavior, double, designWavelength)                  = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, fresnelZOffset)                    = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, designSagittalEntranceArmLength)   = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, designSagittalExitArmLength)       = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, designMeridionalEntranceArmLength) = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, designMeridionalExitArmLength)     = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, designAlphaAngle)                  = 0.0;
    RAYX_PROPERTY(RzpBehavior, double, designBetaAngle)                   = 0.0;
};

struct CrystalBehavior {
    // TODO: find out which properties need validation
    // TODO: find out which properties can use more specific types (e.g., enums, units)
    RAYX_PROPERTY(CrystalBehavior, double, dSpacing2)            = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, unitCellVolume)       = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, offsetAngle)          = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, structureFactorReF0)  = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, structureFactorImF0)  = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, structureFactorReFH)  = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, structureFactorImFH)  = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, structureFactorReFHC) = 0.0;
    RAYX_PROPERTY(CrystalBehavior, double, structureFactorImFHC) = 0.0;
};

using Behavior = std::variant<DetectBehavior, AbsorbBehavior, ReflectBehavior, TransmitBehavior, GratingBehavior, RzpBehavior, CrystalBehavior>;

}  // namespace rayx
