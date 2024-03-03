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

    
    de->setMisalignment(parser.parseMisalignment());
    de->setNumberOfRays(parser.parseNumberRays());
    de->setWorldOrientation(parser.parseOrientation());
    de->setWorldPosition(parser.parsePosition());


    de->setSeperateEnergies(1);
    
}

void setDefaultEnergy(xml::Parser parser, DesignSource* de){
    de->setEnergy(parser.parsePhotonEnergy());
    de->setEnergyDistributionFile(parser.parseEnergyDistributionFile());
    de->setEnergyDistributionType(parser.parseEnergyDistributionType());
    de->setEnergySpread(parser.parseEnergySpread());
    de->setEnergySpreadType(parser.parseEnergySpreadType());

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
    RAYX_LOG << "in set pointsource ";

    setAllMandatory(parser, de);
    RAYX_LOG << "in set pointsource 1";
    setDefaultEnergy(parser, de);
    RAYX_LOG << "in set pointsource 2";
    setDefaultPosition(parser, de);
    RAYX_LOG << "in set pointsource 3";
    setDefaultOrientation(parser, de);
    RAYX_LOG << "in set pointsource 4";

}





}  // namespace RAYX