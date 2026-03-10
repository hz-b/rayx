#pragma once

#include "Angle.h"
#include "Behavior.h"
#include "Material.h"
#include "Coating.h"
#include "Design/Behavior.h"
#include "Model/Behavior.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<DetectBehavior> {
    static model::DetectBehavior apply(const DetectBehavior&) { return model::DetectBehavior{}; }
};

template <>
struct ToModel<AbsorbBehavior> {
    static model::AbsorbBehavior apply(const AbsorbBehavior&) { return model::AbsorbBehavior{}; }
};

template <>
struct ToModel<IgnoreBehavior> {
    static model::IgnoreBehavior apply(const IgnoreBehavior&) { return model::IgnoreBehavior{}; }
};

template <>
struct ToModel<ReflectBehavior> {
    static model::ReflectBehavior apply(const ReflectBehavior& behavior) {
        return model::ReflectBehavior{
            .substrate = toModel(behavior.substrate()),
            .coating   = toModel(behavior.coating()),
        };
    }
};

template <>
struct ToModel<TransmitBehavior> {
    static model::TransmitBehavior apply(const TransmitBehavior& behavior) {
        return model::TransmitBehavior{
            .substrate          = toModel(behavior.substrate()),
            .substrateThickness = toModel(behavior.substrateThickness()),
            .substrateRoughness = toModel(behavior.substrateRoughness()),
        };
    }
};

template <>
struct ToModel<GratingBehavior> {
    static model::GratingBehavior apply(const GratingBehavior& behavior) {
        return model::GratingBehavior{
            .orderOfDiffraction = toModel(behavior.orderOfDiffraction()),
            .lineDensity        = toModel(behavior.lineDensity()),
            .vls                = toModel(behavior.vls()),
        };
    }
};

template <>
struct ToModel<RzpBehavior> {
    static model::RzpBehavior apply(const RzpBehavior& behavior) {
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
};

template <>
struct ToModel<CrystalBehavior> {
    static model::CrystalBehavior apply(const CrystalBehavior& behavior) {
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
};

template <>
struct ToModel<Behavior> {
    static model::Behavior apply(const Behavior& behavior) {
        return std::visit([](const auto& b) -> model::Behavior { return toModel(b); }, behavior);
    }
};

}  // namespace rayx::detail
