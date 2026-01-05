#pragma once

#include <nlohmann/json.hpp>

#include "Design/Behaviour.h"
#include "Design/Material.h" 
#include "Design/Coating.h"

// NOTE: registration of types for JSON serialization must be done in the same namespace as the type is defined in
// thus, we cannot put them in namespace detail here

/* Ai prompt to generate:
Register all types in the following files for JSON serialization using nlohmann::json library:
`Behavior.h`
`Material.h`
`Coating.h`
using `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS`
*/

namespace rayx {

// Behavior.h types
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(DetectorBehavior)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(AbsorbBehavior, transmittance)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(ReflectBehavior, substrate, coating)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(TransmitBehavior, substrate, perfectEfficiency, substrateThickness, substrateRoughness, coating)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(GratingBehavior, vls, lineDensity, orderOfDiffraction)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(RzpBehavior, imageType, rzpType, derivationMethod, designOrderOfDiffraction, orderOfDiffraction, additionalOrder, designWavelength, fresnelZOffset, designSagittalEntranceArmLength, designSagittalExitArmLength, designMeridionalEntranceArmLength, designMeridionalExitArmLength, designAlphaAngle, designBetaAngle)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(CrystalBehavior, dSpacing2, unitCellVolume, offsetAngle, structureFactorReF0, structureFactorImF0, structureFactorReFH, structureFactorImFH, structureFactorReFHC, structureFactorImFHC)

// Coating.h types
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(SingleLayerCoating, material, thickness, roughness)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULTS(RepeatedCoating, layers, numInstances)

// Material.h - Material is a variant, so no direct registration needed
// The variant will be handled automatically by nlohmann/json for AtomicNumber (int) and ChemicalFormula (string)

}  // namespace rayx
