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

void getImageplane(xml::Parser parser, DesignElement* de, Value map) {
    setAllMandatory(parser, de);
}

void getSlit(xml::Parser parser, DesignElement* de, Value map) {
    

    de->setOpeningShape(parser.parseOpeningShape());
    de->setOpeningWidth(parser.parseOpeningWidth());
    de->setOpeningHeight(parser.parseOpeningHeight());
    de->setCentralBeamstop(parser.parseCentralBeamstop());
    de->setStopWidth(parser.parseTotalWidthStop());
    de->setStopHeight(parser.parseTotalHeightStop());
    de->setTotalWidth(parser.parseTotalWidth());
    de->setTotalHeight(parser.parseTotalHeight());
    de->setDistancePreceding(parser.parseDistancePreceding());


    Cutout myCutout = parser.parseCutout(DesignPlane::XY);
    map["geometricalShape"]["type"] = myCutout.m_type;
    if (myCutout.m_type == 0) {
        RectCutout rect = deserializeRect(myCutout);
        map["CutoutWidth"] = rect.m_width;
        map["CutoutLength"] = rect.m_length;
    } else if (myCutout.m_type == 1) {
        EllipticalCutout elli = deserializeElliptical(myCutout);
        map["CutoutDiameterX"] = elli.m_diameter_x;
        map["CutoutDiameterZ"] = elli.m_diameter_z;
    } else if (myCutout.m_type == 2) {
        TrapezoidCutout trapi = deserializeTrapezoid(myCutout);
        map["CutoutWidthA"] = trapi.m_widthA;
        map["CutoutWidthB"] = trapi.m_widthB;
        map["CutoutLength"] = trapi.m_length;
    }

    de->v = map;
    setAllMandatory(parser, de);

}

}  // namespace RAYX