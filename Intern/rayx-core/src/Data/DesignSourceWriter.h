#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shader/Cutout.h"

namespace RAYX {

void setAllMandatory(xml::Parser parser, DesignSource* de, DesignPlane dp) {

}

void setDefaultEnergy(){

}

void setDefaultOrientation(){

}


void setDefaultPOsition(){

}

void setStokes(){
    //de->setStokeslin0(parser.parseLinearPol0());
    //de->setStokeslin45(parser.parseLinearPol45());
    //de->setStokescirc(parser.parseCircularPol());
}






}  // namespace RAYX