#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Beamline/Definitions.h"
#include "Beamline/StringConversion.h"
#include "Core.h"
#include "Element/Cutout.h"

namespace RAYX {

/**
 * Set all Parameters for each optical Element a RML file can define.
 * Set all mandatory parameters is called for every element without exception.
 * This only works in combi with the xml parser.
 */

void setAllMandatory(xml::Parser parser, DesignElement* de, DesignPlane dp) {
    de->setName(parser.name());
    de->setType(parser.type());

    de->setPosition(parser.parsePosition());
    de->setOrientation(parser.parseOrientation());

    de->setSlopeError(parser.parseSlopeError());
    de->setAzimuthalAngle(parser.parseAzimuthalAngle());
    de->setMaterial(parser.parseMaterial());

    de->setDesignPlane(dp);
    if (de->getType() != ElementType::ImagePlane) {
        de->setCutout(parser.parseCutout(dp, ElementTypeToString.at(parser.type())));
    } else {
        de->setCutout(parser.parseCutout(dp, ElementTypeToString.at(parser.type())));
    }
}

void getImageplane(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XY);
    de->setCurvatureType(CurvatureType::Plane);
    de->setBehaviourType(BehaviourType::ImagePlane);
}

void getSlit(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XY);
    de->setCurvatureType(CurvatureType::Plane);
    de->setBehaviourType(BehaviourType::Slit);

    de->setOpeningShape(static_cast<CutoutType>(parser.parseOpeningShape()));
    de->setOpeningWidth(parser.parseOpeningWidth());
    de->setOpeningHeight(parser.parseOpeningHeight());
    de->setCentralBeamstop(parser.parseCentralBeamstop());
    de->setStopWidth(parser.parseTotalWidthStop());
    de->setStopHeight(parser.parseTotalHeightStop());
    de->setTotalWidth(parser.parseTotalWidth());
    de->setTotalHeight(parser.parseTotalHeight());
    de->setDistancePreceding(parser.parseDistancePreceding());
}

void getCone(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Cone);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
    de->setTotalLength(parser.parseTotalLength());
}

void getCrystal(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Plane);
    de->setBehaviourType(BehaviourType::Crystal);

    de->setCrystalType(parser.parseCrystalType());
    de->setOffsetAngle(parser.parseOffsetAngle());
    de->setStructureFactorReF0(parser.parseStructureFactorReF0());
    de->setStructureFactorImF0(parser.parseStructureFactorImF0());
    de->setStructureFactorReFH(parser.parseStructureFactorReFH());
    de->setStructureFactorImFH(parser.parseStructureFactorImFH());
    de->setStructureFactorReFHC(parser.parseStructureFactorReFHC());
    de->setStructureFactorImFHC(parser.parseStructureFactorImFHC());
    de->setUnitCellVolume(parser.parseUnitCellVolume());
    de->setDSpacing2(parser.parseDSpacing2());
}

void getCylinder(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Cylinder);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setRadiusDirection(parser.parseBendingRadius());
    de->setRadius(parser.parseRadius());
    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
}

void getEllipsoid(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Ellipsoid);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setShortHalfAxisB(parser.parseShortHalfAxisB());
    de->setLongHalfAxisA(parser.parseLongHalfAxisA());
    de->setDesignGrazingIncAngle(parser.parseDesignGrazingIncAngle());
    de->setFigureRotation(parser.parseFigureRotation());
    de->setParameterA11(parser.parseParameterA11());

    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
}

void getParaboloid(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Paraboloid);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setArmLength(parser.parseArmLength());
    de->setParameterP(parser.parseParameterP());
    de->setParameterPType(parser.parseParameterPType());
    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setParameterA11(parser.parseParameterA11());
}

void getGrating(xml::Parser parser, DesignElement* de) {
    de->setBehaviourType(BehaviourType::Grating);
    de->setVLSParameters(parser.parseVls());
    de->setLineDensity(parser.parseLineDensity());
    de->setOrderOfDiffraction(parser.parseOrderDiffraction());
}

void getPlaneGrating(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Plane);

    getGrating(parser, de);
}

void getSphereGrating(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Spherical);

    de->setDeviationAngle(parser.parseDeviationAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
    de->setRadius(parser.parseRadius());

    getGrating(parser, de);
}

void getPlaneMirror(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Plane);
    de->setBehaviourType(BehaviourType::Mirror);
}

void getSphereMirror(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Spherical);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
    de->setRadius(parser.parseRadius());
}

void getToroidMirror(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Toroidal);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setShortRadius(parser.parseShortRadius());
    de->setLongRadius(parser.parseLongRadius());
}

void getToroidalGrating(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Toroidal);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setShortRadius(parser.parseShortRadius());
    de->setLongRadius(parser.parseLongRadius());

    getGrating(parser, de);
}

void getRZP(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    CurvatureType curv = parser.parseCurvatureType();
    if (curv == CurvatureType::Spherical) {  // special case of sphere
        curv = CurvatureType::RzpSphere;
    }
    de->setCurvatureType(curv);
    de->setBehaviourType(BehaviourType::Rzp);

    de->setFresnelZOffset(parser.parseFresnelZOffset());
    de->setDesignMeridionalExitArmLength(parser.parseExitArmLengthMer());
    de->setDesignMeridionalEntranceArmLength(parser.parseEntranceArmLengthMer());
    de->setDesignSagittalEntranceArmLength(parser.parseEntranceArmLengthSag());
    de->setDesignSagittalExitArmLength(parser.parseExitArmLengthSag());
    de->setDesignEnergy(parser.parseDesignEnergy());
    de->setLongRadius(parser.parseLongRadius());
    de->setDesignOrderOfDiffraction(parser.parseDesignOrderDiffraction());
    de->setOrderOfDiffraction(parser.parseOrderDiffraction());
    de->setDesignAlphaAngle(parser.parseDesignAlphaAngle());
    de->setDesignBetaAngle(parser.parseDesignBetaAngle());
    de->setImageType(parser.parseImageType());
    de->setAdditionalOrder(parser.parseAdditionalOrder());
    de->setShortRadius(parser.parseShortRadius());
    de->setLongRadius(parser.parseLongRadius());
}

void getExpertsCubic(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Cubic);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setExpertsCubic(serializeCubic(parser.parseCubicParameters()));
}

void getExpertsOptics(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    de->setCurvatureType(CurvatureType::Quadric);
    de->setBehaviourType(BehaviourType::Mirror);

    de->setExpertsOptics(serializeQuadric(parser.parseQuadricParameters()));
}

void getFoil(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XY);
    de->setCurvatureType(CurvatureType::Plane);
    de->setBehaviourType(BehaviourType::Foil);

    de->setThicknessSubstrate(parser.parseThicknessSubstrate());
    de->setRoughnessSubstrate(parser.parseRoughnessSubstrate());
}

}  // namespace RAYX
