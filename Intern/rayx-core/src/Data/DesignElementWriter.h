#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shader/Cutout.h"

namespace RAYX {

void setAllMandatory(xml::Parser parser, DesignElement* de) {
    de->setName(parser.name());

    de->setWorldPosition(parser.parsePosition());
    de->setWorldOrientation(parser.parseOrientation());
    de->setMisalignment(parser.parseMisalignment());

    de->setSlopeError(parser.parseSlopeError());
    de->setAzimuthalAngle(parser.parseAzimuthalAngle());
    de->setMaterial(parser.parseMaterial());
}

void getImageplane(xml::Parser parser, DesignElement* de) {
    setAllMandatory(parser, de);
}

void getSlit(xml::Parser parser, DesignElement* de) {
    
    setAllMandatory(parser, de);

    de->setOpeningShape(parser.parseOpeningShape());
    de->setOpeningWidth(parser.parseOpeningWidth());
    de->setOpeningHeight(parser.parseOpeningHeight());
    de->setCentralBeamstop(parser.parseCentralBeamstop());
    de->setStopWidth(parser.parseTotalWidthStop());
    de->setStopHeight(parser.parseTotalHeightStop());
    de->setTotalWidth(parser.parseTotalWidth());
    de->setTotalHeight(parser.parseTotalHeight());
    de->setDistancePreceding(parser.parseDistancePreceding());
    de->setCutout(parser.parseCutout(DesignPlane::XY));
}

}  // namespace RAYX