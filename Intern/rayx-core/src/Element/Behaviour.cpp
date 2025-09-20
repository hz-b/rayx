#include "Behaviour.h"

#include "Beamline/Beamline.h"
#include "Beamline/Definitions.h"
#include "Design/DesignElement.h"
#include "Shader/Utils.h"

namespace RAYX {

Behaviour makeBehaviour(const DesignElement& dele) {
    switch (dele.getBehaviourType()) {
        case BehaviourType::Crystal:
            return makeCrystal(dele);
        case BehaviourType::Grating:
            return makeGrating(dele);
        case BehaviourType::Mirror:
            return Behaviour::Mirror{};
        case BehaviourType::Rzp:
            return makeRZPBehaviour(dele);
        case BehaviourType::Slit:
            return makeSlit(dele);
        case BehaviourType::Foil:
            return makeFoil(dele);
        default:
            return Behaviour::ImagePlane{};
    }
}

Behaviour makeCrystal(const DesignElement& dele) {
    auto dSpacing2 = dele.getDSpacing2();
    auto unitCellVoulume = dele.getUnitCellVolume();
    auto offsetAngle = dele.getOffsetAngle();

    auto structureFactorReF0 = dele.getStructureFactorReF0();
    auto structureFactorImF0 = dele.getStructureFactorImF0();
    auto structureFactorReFH = dele.getStructureFactorReFH();
    auto structureFactorImFH = dele.getStructureFactorImFH();
    auto structureFactorReFHC = dele.getStructureFactorReFHC();
    auto structureFactorImFHC = dele.getStructureFactorImFHC();

    return Behaviour::Crystal{
        .m_dSpacing2 = dSpacing2,
        .m_unitCellVolume = unitCellVoulume,
        .m_offsetAngle = offsetAngle.rad,

        .m_structureFactorReF0 = structureFactorReF0,
        .m_structureFactorImF0 = structureFactorImF0,
        .m_structureFactorReFH = structureFactorReFH,
        .m_structureFactorImFH = structureFactorImFH,
        .m_structureFactorReFHC = structureFactorReFHC,
        .m_structureFactorImFHC = structureFactorImFHC,
    };
}

Behaviour makeGrating(const DesignElement& dele) {
    auto vls = dele.getVLSParameters();
    return Behaviour::Grating{
        .m_vls = {vls[0], vls[1], vls[2], vls[3], vls[4], vls[5]},
        .m_lineDensity = dele.getLineDensity(),
        .m_orderOfDiffraction = dele.getOrderOfDiffraction(),
    };
}

Cutout mkOpeningCutout(const DesignElement& dele) {
    auto shape = dele.getOpeningShape();

    // This converts y (height) to z (length), as the RML file uses DesignPlane::XY for slits, whereas our model uses XZ.

    if (shape == CutoutType::Rect) {
        return Cutout::Rect{
            .m_width = dele.getOpeningWidth(),
            .m_length = dele.getOpeningHeight(),
        };
    } else if (shape == CutoutType::Elliptical) {  // elliptical
        return Cutout::Elliptical{
            .m_diameter_x = dele.getOpeningWidth(),
            .m_diameter_z = dele.getOpeningHeight(),
        };
    } else {
        RAYX_EXIT << "unsupported opening type!";
        return Cutout::Unlimited{};
    }
}

Cutout mkBeamstopCutout(const DesignElement& dele) {
    auto centralBeamstop = dele.getCentralBeamstop();

    if (centralBeamstop == CentralBeamstop::None) {
        return Cutout::Rect{
            .m_width = 0,
            .m_length = 0,
        };
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        return Cutout::Elliptical{
            .m_diameter_x = dele.getStopWidth(),
            .m_diameter_z = dele.getStopHeight(),
        };
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        return Cutout::Rect{
            .m_width = dele.getStopWidth(),
            .m_length = dele.getStopHeight(),
        };
    } else {
        RAYX_EXIT << "unsupported CentralBeamstop type!";
        return Cutout::Unlimited{};
    }
}

Behaviour makeSlit(const DesignElement& dele) {
    return Behaviour::Slit{
        .m_openingCutout = mkOpeningCutout(dele),
        .m_beamstopCutout = mkBeamstopCutout(dele),
    };
}

Behaviour makeRZPBehaviour(const DesignElement& dele) {
    auto fresnelZOffset = dele.getFresnelZOffset();
    auto designAlphaAngle = dele.getDesignAlphaAngle();
    auto designBetaAngle = dele.getDesignBetaAngle();
    auto designOrderOfDiffraction = dele.getDesignOrderOfDiffraction();
    auto designEnergy = dele.getDesignEnergy();
    auto designSagittalEntranceArmLength = dele.getDesignSagittalEntranceArmLength();
    auto designSagittalExitArmLength = dele.getDesignSagittalExitArmLength();
    auto designMeridionalEntranceArmLength = dele.getDesignMeridionalEntranceArmLength();  // Assuming it should be the same as sagittal
    auto designMeridionalExitArmLength = dele.getDesignMeridionalExitArmLength();
    auto orderOfDiffraction = dele.getOrderOfDiffraction();  // Assuming this getter is available

    // designEnergy = designEnergy; // if Auto == true, take energy of Source
    // (param sourceEnergy), else designEnergy = designEnergy
    auto designWavelength = designEnergy == 0 ? 0 : energyToWaveLength(designEnergy);
    auto additionalOrder = double(dele.getAdditionalOrder());

    auto imageType = dele.getImageType();

    return Behaviour::RZP{.m_imageType = (double)imageType,
                          .m_rzpType = (double)RZPType::Elliptical,
                          .m_derivationMethod = 0,
                          .m_designWavelength = designWavelength,
                          .m_designOrderOfDiffraction = designOrderOfDiffraction,
                          .m_orderOfDiffraction = orderOfDiffraction,
                          .m_fresnelZOffset = fresnelZOffset,
                          .m_designSagittalEntranceArmLength = designSagittalEntranceArmLength,
                          .m_designSagittalExitArmLength = designSagittalExitArmLength,
                          .m_designMeridionalEntranceArmLength = designMeridionalEntranceArmLength,
                          .m_designMeridionalExitArmLength = designMeridionalExitArmLength,
                          .m_designAlphaAngle = designAlphaAngle.rad,
                          .m_designBetaAngle = designBetaAngle.rad,
                          .m_additionalOrder = (double)additionalOrder};
}

// Foil Behaviour
Behaviour makeFoil(const DesignElement& dele) {
    auto thicknessSubstrate = dele.getThicknessSubstrate();
    auto roughnessSubstrate = dele.getRoughnessSubstrate();

    return Behaviour::Foil{
        .m_thicknessSubstrate = thicknessSubstrate,
        .m_roughnessSubstrate = roughnessSubstrate,
    };
}

}  // namespace RAYX
