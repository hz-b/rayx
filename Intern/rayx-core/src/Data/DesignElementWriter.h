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
    de->setSlopeError(parser.parseSlopeError());
    de->setAzimuthalAngle(parser.parseAzimuthalAngle());
    de->setMaterial(parser.parseMaterial());
}
void getMisalignment(xml::Parser parser, DesignElement* de, Value map) {
    map["rotationXerror"] = parser.parseMisalignment().m_rotationXerror.rad;
    map["rotationYerror"] = parser.parseMisalignment().m_rotationYerror.rad;
    map["rotationZerror"] = parser.parseMisalignment().m_rotationZerror.rad;

    map["translationXerror"] = parser.parseMisalignment().m_translationXerror;
    map["translationYerror"] = parser.parseMisalignment().m_translationYerror;
    map["translationZerror"] = parser.parseMisalignment().m_translationZerror;
}

void getImageplane(xml::Parser parser, DesignElement* de, Value map) {
    setAllMandatory(parser, de);
    de->setWorldPosition(parser.parsePosition());
    de->setWorldOrientation(parser.parseOrientation());
}
void getSlit(xml::Parser parser, DesignElement* de, Value map) {
    const char* CentralBeamStop[] = {"None", "Rectangle", "Elliptical"};
    

    map["openingShape"] = parser.parseOpeningShape();
    map["openingWidth"] = parser.parseOpeningWidth();
    map["openingHeight"] = parser.parseOpeningHeight();
    map["centralBeamstop"] = CentralBeamStop[(int)parser.parseCentralBeamstop()];
    map["stopWidth"] = parser.parseTotalWidthStop();
    map["stopHeight"] = parser.parseTotalHeightStop();
    map["totalWidth"] = parser.parseTotalWidth();
    map["totalHeight"] = parser.parseTotalHeight();
    map["distancePreceding"] = parser.parseDistancePreceding();
    map["azimuthalAngle"] = parser.parseAzimuthalAngle().rad;

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
    de->setName(parser.name());
    de->setWorldPosition(parser.parsePosition());
    de->setWorldOrientation(parser.parseOrientation());
    getMisalignment(parser, de, map);

}

}  // namespace RAYX