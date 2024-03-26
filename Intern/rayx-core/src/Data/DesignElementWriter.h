#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shader/Cutout.h"

namespace RAYX {

void setAllMandatory(xml::Parser parser, DesignElement* de, DesignPlane dp) {
    de->setName(parser.name());
    de->setType(parser.type());

    de->setWorldPosition(parser.parsePosition());
    de->setWorldOrientation(parser.parseOrientation());
    de->setMisalignment(parser.parseMisalignment());

    de->setSlopeError(parser.parseSlopeError());
    de->setAzimuthalAngle(parser.parseAzimuthalAngle());
    de->setMaterial(parser.parseMaterial());
    de->setCurvatureType(CurvatureType::Plane);

    if (de->getType() != "ImagePlane") {
        de->setCutout(parser.parseCutout(dp));
    } else {
        de->setCutout(serializeUnlimited());
    }
}

void getImageplane(xml::Parser parser, DesignElement* de) { setAllMandatory(parser, de, DesignPlane::XY); }

void getSlit(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XY);

    de->setOpeningShape(parser.parseOpeningShape());
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

    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
    de->setTotalLength(parser.parseTotalLength());
}

void getCylinder(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setRadiusDirection(parser.parseBendingRadius());
    de->setRadius(parser.parseRadius());
    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
}

void getEllipsoid(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

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

    de->setArmLength(parser.parseArmLength());
    de->setParameterP(parser.parseParameterP());
    de->setParameterPType(parser.parseParameterPType());
    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setParameterA11(parser.parseParameterA11());
}

void getGrating(xml::Parser parser, DesignElement* de) {
    de->setVLSParameters(parser.parseVls());
    de->setLineDensity(parser.parseLineDensity());
    de->setOrderOfDiffraction(parser.parseOrderDiffraction());
}

void getPlaneGrating(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
    getGrating(parser, de);
}

void getSphereGrating(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setRadius(parser.parseRadius());
    getGrating(parser, de);
}

void getPlaneMirror(xml::Parser parser, DesignElement* de) { setAllMandatory(parser, de, DesignPlane::XZ); }

void getSphereMirror(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
}

void getToroidMirror(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setShortRadius(parser.parseShortRadius());
    de->setLongRadius(parser.parseLongRadius());
}

void getRZP(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

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
    de->setCurvatureType(parser.parseCurvatureType());
    de->setAdditionalOrder(parser.parseAdditionalOrder());
    de->setShortRadius(parser.parseShortRadius());
    de->setLongRadius(parser.parseLongRadius());
    de->setCurvatureType(parser.parseCurvatureType());
}

void getExpertsCubic(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setExpertsCubic(serializeCubic(parser.parseCubicParameters()));
}

void getExpertsOptics(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setExpertsOptics(serializeQuadric(parser.parseQuadricParameters()));
}

}  // namespace RAYX