#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Cutout.h"
#include "Beamline/Beamline.h"

namespace RAYX {

void getWorldPosition(xml::Parser parser, DesignElement* de){
    de->v["worldPosition"]["x"] = parser.parsePosition().x;
    de->v["worldPosition"]["y"] = parser.parsePosition().y;
    de->v["worldPosition"]["z"] = parser.parsePosition().z;

    de->v["worldXDirection"]["x"] = parser.parseOrientation()[1][1];
    de->v["worldXDirection"]["y"] = parser.parseOrientation()[1][2];
    de->v["worldXDirection"]["z"] = parser.parseOrientation()[1][3];

    de->v["worldYDirection"]["x"] = parser.parseOrientation()[2][1];
    de->v["worldYDirection"]["y"] = parser.parseOrientation()[2][2];
    de->v["worldYDirection"]["z"] = parser.parseOrientation()[2][3];

    de->v["worldZDirection"]["x"] = parser.parseOrientation()[3][1];
    de->v["worldZDirection"]["y"] = parser.parseOrientation()[3][2];
    de->v["worldZDirection"]["z"] = parser.parseOrientation()[3][3];
}

void getMisallignment(xml::Parser parser, DesignElement* de){


    de->v["rotationXerror"] = parser.parseMisalignment().m_rotationXerror.rad;
    de->v["rotationYerror"] = parser.parseMisalignment().m_rotationYerror.rad;
    de->v["rotationZerror"] = parser.parseMisalignment().m_rotationZerror.rad;

    de->v["translationXerror"] = parser.parseMisalignment().m_translationXerror;
    de->v["translationYerror"] = parser.parseMisalignment().m_translationYerror;
    de->v["translationZerror"] = parser.parseMisalignment().m_translationZerror;

}

void getImageplane(xml::Parser parser, DesignElement* de) {
    
    de->v["name"] = parser.name();
    de->v["distanceImagePlane"] = parser.parseDistancePreceding();
    
    getWorldPosition(parser, de);
}
void getSlit(xml::Parser parser, DesignElement* de) {
    const char * CentralBeamStop[] = { "None", "Rectangle", "Elliptical"};
    de->v["name"] = parser.name();

    de->v["openingShape"] = parser.parseOpeningShape();
    de->v["openingWidth"] = parser.parseOpeningWidth();
    de->v["openingHeight"] = parser.parseOpeningHeight();
    de->v["centralBeamstop"] = CentralBeamStop[(int) parser.parseCentralBeamstop()];
    de->v["stopWidth"] = parser.parseTotalWidthStop();
    de->v["stopHeight"] = parser.parseTotalHeightStop();
    de->v["totalWidth"] = parser.parseTotalWidth();
    de->v["totalHeight"] = parser.parseTotalHeight();
    de->v["distancePreceding"] = parser.parseDistancePreceding();
    de->v["azimuthalAngle"] = parser.parseAzimuthalAngle().rad;

    Cutout myCutout = parser.parseCutout(DesignPlane::XY);
    de->v["geometricalShape"]["type"] = myCutout.m_type;
    if (myCutout.m_type == 0){
        RectCutout rect = deserializeRect(myCutout);
        de->v["CutoutWidth"] = rect.m_width;
        de->v["CutoutLength"] = rect.m_length;
    }else if (myCutout.m_type == 1){
        EllipticalCutout elli = deserializeElliptical(myCutout);
        de->v["CutoutDiameterX"] = elli.m_diameter_x;
        de->v["CutoutDiameterZ"] = elli.m_diameter_z;
    }else if (myCutout.m_type == 2){
        TrapezoidCutout trapi = deserializeTrapezoid(myCutout);
        de->v["CutoutWidthA"] = trapi.m_widthA;
        de->v["CutoutWidthB"] = trapi.m_widthB;
        de->v["CutoutLength"] = trapi.m_length;
    }    

    getWorldPosition(parser, de);
    getMisallignment(parser, de);
}

}