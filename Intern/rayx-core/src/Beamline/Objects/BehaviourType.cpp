#include "BehaviourType.h"

#include "DesignElement/DesignElement.h"
#include "Beamline/Beamline.h"
#include "Shader/Utils.h"

namespace RAYX{

Behaviour makeBehaviour(const DesignElement& dele){
    BehaviourType behave = dele.getBehaviourType();
    if (behave == BehaviourType::Grating) {
        return makeGrating(dele);
    } else if (behave == BehaviourType::Mirror) {
        return serializeMirror();
    } else if (behave == BehaviourType::Rzp) {
        return makeRZPBehaviour(dele);
    } else if (behave == BehaviourType::Slit) {
        return makeSlit(dele);
    } else if (behave == BehaviourType::Mirror){
        return serializeMirror();
    } else {
        return serializeImagePlane();
    }
}

Behaviour makeGrating(const DesignElement& dele) {
    auto vls = dele.getVLSParameters();
    return serializeGrating({
        .m_vls = {vls[0], vls[1], vls[2], vls[3], vls[4], vls[5]},
        .m_lineDensity = dele.getLineDensity(),
        .m_orderOfDiffraction = dele.getOrderOfDiffraction(),
    });
}

Cutout mkOpeningCutout(const DesignElement& dele) {
    auto shape = dele.getOpeningShape();

    // This converts y (height) to z (length), as the RML file uses DesignPlane::XY for slits, whereas our model uses XZ.

    if (shape == CTYPE_RECT) {
        return serializeRect({
            .m_width = dele.getOpeningWidth(),
            .m_length = dele.getOpeningHeight(),
        });
    } else if (shape == CTYPE_ELLIPTICAL) { // elliptical
        return serializeElliptical({
            .m_diameter_x = dele.getOpeningWidth(),
            .m_diameter_z = dele.getOpeningHeight(),
        });
    } else {
        RAYX_ERR << "unsupported opening type!";
        return {};
    }
}

Cutout mkBeamstopCutout(const DesignElement& dele) {
    auto centralBeamstop = dele.getCentralBeamstop();

    if (centralBeamstop == CentralBeamstop::None) {
        return serializeRect({
            .m_width = 0,
            .m_length = 0,
        });
    } else if (centralBeamstop == CentralBeamstop::Elliptical) {
        return serializeElliptical({
            .m_diameter_x = dele.getStopWidth(),
            .m_diameter_z = dele.getStopHeight(),
        });
    } else if (centralBeamstop == CentralBeamstop::Rectangle) {
        return serializeRect({
            .m_width = dele.getStopWidth(),
            .m_length = dele.getStopHeight(),
        });
    } else {
        RAYX_ERR << "unsupported CentralBeamstop type!";
        return {};
    }
}

Behaviour makeSlit(const DesignElement& dele) {
    return serializeSlit({
        .m_openingCutout = mkOpeningCutout(dele),
        .m_beamstopCutout = mkBeamstopCutout(dele),
    });
}


Behaviour makeRZPBehaviour(const DesignElement& dele){
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
    auto designWavelength = designEnergy == 0 ? 0 : hvlam(designEnergy);
    auto additionalOrder = double(dele.getAdditionalOrder());


    auto imageType = dele.getImageType();


    return serializeRZP({.m_imageType = (double) imageType,
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
                                   .m_additionalOrder = (double)additionalOrder});

}

}