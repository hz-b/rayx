#include "ReflectionZonePlate.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shader/Constants.h"
#include "Shader/Utils.h"

namespace RAYX {

Element makeReflectionZonePlate(const DesignElement& dele) {
    auto fresnelZOffset = dele.getFresnelZOffset();
    auto designAlphaAngle = dele.getDesignAlphaAngle();
    auto designBetaAngle = dele.getDesignBetaAngle();
    auto designOrderOfDiffraction = dele.getDesignOrderOfDiffraction();
    auto designEnergy = dele.getDesignEnergy();
    auto designSagittalEntranceArmLength = dele.getDesignSagittalEntranceArmLength();
    auto designSagittalExitArmLength = dele.getDesignSagittalExitArmLength();
    auto designMeridionalEntranceArmLength = dele.getDesignMeridionalEntranceArmLength();
    auto designMeridionalExitArmLength = dele.getDesignMeridionalExitArmLength();
    auto orderOfDiffraction = dele.getOrderOfDiffraction();


    // designEnergy = designEnergy; // if Auto == true, take energy of Source
    // (param sourceEnergy), else designEnergy = designEnergy
    auto designWavelength = designEnergy == 0 ? 0 : hvlam(designEnergy);
    auto additionalOrder = double(dele.getAdditionalOrder());

    auto curvatureType = dele.getCurvatureType();
    Surface surface;

    // set parameters in Quadric class
    if (curvatureType == CurvatureType::Plane) {
        surface = makePlane();
    } else if (curvatureType == CurvatureType::Toroidal) {
        surface = makeToroid(dele);
    } else if (curvatureType == CurvatureType::Spherical) {
        surface = makeSphere(dele.getLongRadius());
    } else {
        RAYX_ERR << "invalid curvature Type";
    }

    auto imageType = dele.getImageType();

    auto behaviour = serializeRZP({.m_imageType = (double) imageType,
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
    return makeElement(dele, behaviour, surface);
}

}  // namespace RAYX
