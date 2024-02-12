#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Core.h"
#include "Beamline/Beamline.h"

namespace RAYX {


void getImageplane(xml::Parser parser, DesignElement* de) {
    
    de->v["name"] = parser.name();
    de->v["distanceImagePlane"] = parser.parseDistancePreceding();
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


    //de->v["geometricalShape"] = parser.parse
    
}



}