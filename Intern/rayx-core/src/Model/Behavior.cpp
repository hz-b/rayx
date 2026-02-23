#include "Behavior.h"

namespace rayx::detail {

model::DetectBehavior toModel(const DetectBehavior&) { return model::DetectBehavior{}; }

model::AbsorbBehavior toModel(const AbsorbBehavior&) { return model::AbsorbBehavior{}; }

model::IgnoreBehavior toModel(const IgnoreBehavior&) { return model::IgnoreBehavior{}; }

model::ReflectBehavior toModel(const ReflectBehavior& behavior) {
    return model::ReflectBehavior{
        .substrate = behavior.substrate(),
        .coating   = toModel(behavior.coating()),
    };
}

model::TransmitBehavior toModel(const TransmitBehavior& behavior) {
    return model::TransmitBehavior{
        .substrate          = behavior.substrate(),
        .substrateThickness = behavior.substrateThickness(),
        .substrateRoughness = behavior.substrateRoughness(),
    };
}

model::GratingBehavior toModel(const GratingBehavior& behavior) {
    return model::GratingBehavior{
        .orderOfDiffraction = behavior.orderOfDiffraction(),
        .lineDensity        = behavior.lineDensity(),
        .vls                = {behavior.vls()[0], behavior.vls()[1], behavior.vls()[2], behavior.vls()[3], behavior.vls()[4], behavior.vls()[5]},
    };
}

model::RzpBehavior toModel(const RzpBehavior& behavior) {
    return model::RzpBehavior{
        .imageType                         = behavior.imageType(),
        .rzpType                           = behavior.rzpType(),
        .derivationMethod                  = behavior.derivationMethod(),
        .designOrderOfDiffraction          = behavior.designOrderOfDiffraction(),
        .orderOfDiffraction                = behavior.orderOfDiffraction(),
        .additionalOrder                   = behavior.additionalOrder(),
        .designWavelength                  = behavior.designWavelength(),
        .fresnelZOffset                    = behavior.fresnelZOffset(),
        .designSagittalEntranceArmLength   = behavior.designSagittalEntranceArmLength(),
        .designSagittalExitArmLength       = behavior.designSagittalExitArmLength(),
        .designMeridionalEntranceArmLength = behavior.designMeridionalEntranceArmLength(),
        .designMeridionalExitArmLength     = behavior.designMeridionalExitArmLength(),
        .designAlphaAngle                  = behavior.designAlphaAngle(),
        .designBetaAngle                   = behavior.designBetaAngle(),
    };
}

model::CrystalBehavior toModel(const CrystalBehavior& behavior) {
    return model::CrystalBehavior{
        .dSpacing2            = behavior.dSpacing2(),
        .unitCellVolume       = behavior.unitCellVolume(),
        .offsetAngle          = behavior.offsetAngle(),
        .structureFactorReF0  = behavior.structureFactorReF0(),
        .structureFactorImF0  = behavior.structureFactorImF0(),
        .structureFactorReFH  = behavior.structureFactorReFH(),
        .structureFactorImFH  = behavior.structureFactorImFH(),
        .structureFactorReFHC = behavior.structureFactorReFHC(),
        .structureFactorImFHC = behavior.structureFactorImFHC(),
    };
}

model::Behavior toModel(const Behavior& behavior) {
    return std::visit([](const auto& b) -> model::Behavior { return toModel(b); }, behavior);
}

}  // namespace rayx::detail
