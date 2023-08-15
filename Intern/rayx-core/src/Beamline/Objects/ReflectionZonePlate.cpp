#include "ReflectionZonePlate.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"

namespace RAYX {

Element makeReflectionZonePlate(const DesignObject& dobj) {
    auto fresnelZOffset = dobj.parseFresnelZOffset();
    auto designAlphaAngle = dobj.parseDesignAlphaAngle();
    auto designBetaAngle = dobj.parseDesignBetaAngle();
    auto designOrderOfDiffraction = dobj.parseDesignOrderDiffraction();
    auto designEnergy = dobj.parseDesignEnergy();
    auto designSagittalEntranceArmLength = dobj.parseEntranceArmLengthSag();
    auto designSagittalExitArmLength = dobj.parseExitArmLengthSag();
    auto designMeridionalEntranceArmLength = dobj.parseEntranceArmLengthSag();
    auto designMeridionalExitArmLength = dobj.parseExitArmLengthMer();
    auto orderOfDiffraction = dobj.parseOrderDiffraction();

    // designEnergy = designEnergy; // if Auto == true, take energy of Source
    // (param sourceEnergy), else designEnergy = designEnergy
    auto designWavelength = designEnergy == 0 ? 0 : hvlam(designEnergy);
    auto additionalOrder = double(dobj.parseAdditionalOrder());

    auto curvatureType = dobj.parseCurvatureType();
    Surface surface;

    // set parameters in Quadric class
    if (curvatureType == CurvatureType::Plane) {
        surface = makePlane();
    } else if (curvatureType == CurvatureType::Toroidal) {
        surface = makeToroid(dobj);
    } else if (curvatureType == CurvatureType::Spherical) {
        surface = makeSphere(dobj.parseLongRadius());
    } else {
        RAYX_ERR << "invalid curvature Type";
    }

    auto imageType = dobj.parseImageType();

    auto behaviour = serializeRZP({.m_imageType = (double) imageType,
                                   .m_rzpType = (double)RZPType::Elliptical,
                                   .m_derivationMethod = 0,
                                   .m_designWavelength = designWavelength,
                                   .m_curvatureType = (double)curvatureType,
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
    return makeElement(dobj, behaviour, surface);
}

}  // namespace RAYX
