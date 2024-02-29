#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shader/Cutout.h"

namespace RAYX {

void setAllMandatory(xml::Parser parser, DesignSource* de) {
    de->setEnergyDistribution(parser.parseEnergyDistribution());
    de->setMisalignment(parser.parseMisalignment());
    de->setNumberOfRays(parser.parseNumberRays());
    de->setWorldOrientation(parser.parseOrientation());
    de->setWorldPosition(parser.parsePosition());
}

void setDefaultEnergy(xml::Parser parser, DesignSource* de){
    de->setEnergy(parser.parseElectronEnergy());

}

void setDefaultOrientation(xml::Parser parser, DesignSource* de){

    de->setHorDist(parser.parseHorDivDistribution());
    de->setVerDist(parser.parseVerDivDistribution());
    de->setHorDivergence(parser.parseHorDiv());
    de->setVerDivergence(parser.parseVerDiv());
}


void setDefaultPosition(xml::Parser parser, DesignSource* de){
    de->setWidthDist(parser.parseSourceWidthDistribution());
    de->setHeightDist(parser.parseSourceHeightDistribution());
    de->setSourceDepth(parser.parseSourceDepth());
    de->setSourceHeight(parser.parseSourceHeight());
    de->setSourceWidth(parser.parseSourceWidth());
}

void setStokes(xml::Parser parser, DesignSource* de){
    de->setStokeslin0(parser.parseLinearPol0());
    de->setStokeslin45(parser.parseLinearPol45());
    de->setStokescirc(parser.parseCircularPol());
}


void setPointSource(xml::Parser parser, DesignSource* de) {
    setAllMandatory(parser, de);
    setDefaultEnergy(parser, de);
    setDefaultPosition(parser, de);
    setDefaultOrientation(parser, de);
}





}  // namespace RAYX