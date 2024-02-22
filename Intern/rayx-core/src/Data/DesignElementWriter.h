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

    de->setWorldPosition(parser.parsePosition());
    de->setWorldOrientation(parser.parseOrientation());
    de->setMisalignment(parser.parseMisalignment());

    de->setSlopeError(parser.parseSlopeError());
    de->setAzimuthalAngle(parser.parseAzimuthalAngle());
    de->setMaterial(parser.parseMaterial());

    if(de->getName() != "ImagePlane"){ 
        de->setCutout(parser.parseCutout(dp));
    }
}

void getImageplane(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XY);
}

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
    //de->setCutout(parser.parseCutout(DesignPlane::XY));
}


void getCone(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
    de->setTotalLength(parser.parseTotalLength());
    //de->setCutout(parser.parseCutout(DesignPlane::XZ));
}

void getCylinder(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setRadiusDirection(parser.parseBendingRadius());
    de->setRadius(parser.parseRadius());
    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setEntranceArmLength(parser.parseEntranceArmLength());
    de->setExitArmLength(parser.parseExitArmLength());
    //de->setCutout(parser.parseCutout(DesignPlane::XZ));
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

    //de->setCutout(parser.parseCutout(DesignPlane::XZ));
}

void getParaboloid(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setArmLength(parser.parseArmLength());
    de->setParameterP(parser.parseParameterP());
    de->setParameterPType(parser.parseParameterPType());
    de->setGrazingIncAngle(parser.parseGrazingIncAngle());
    de->setParameterA11(parser.parseParameterA11());

    //de->setCutout(parser.parseCutout(DesignPlane::XZ));
}


void getPlaneGrating(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);

    de->setVLSParameters(parser.parseVls());
    de->setLineDensity(parser.parseLineDensity());
    de->setOrderDiffraction(parser.parseOrderDiffraction());
}

void getPlaneMirror(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de, DesignPlane::XZ);
}


}  // namespace RAYX