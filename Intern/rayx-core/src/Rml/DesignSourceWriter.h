#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Element/Cutout.h"

namespace RAYX {

void setAllMandatory(xml::Parser parser, DesignSource* ds) {
    ds->setName(parser.name());
    ds->setType(parser.type());
    ds->setMisalignment(parser.parseMisalignment());
    ds->setNumberOfRays(parser.parseNumberRays());
    ds->setOrientation(parser.parseOrientation());
    ds->setPosition(parser.parsePosition());

    ds->setSeparateEnergies(1);
}

void setDefaultEnergy(xml::Parser parser, DesignSource* ds) {
    ds->setEnergyDistributionType(parser.parseEnergyDistributionType());
    ds->setEnergySpreadType(parser.parseEnergySpreadType());

    if (ds->getEnergyDistributionType() == EnergyDistributionType::File) {
        ds->setEnergyDistributionFile(parser.parseEnergyDistributionFile().generic_string());
    } else {
        ds->setEnergy(parser.parsePhotonEnergy());
        ds->setEnergySpread(parser.parseEnergySpread());
    }
}

void setDefaultOrientation(xml::Parser parser, DesignSource* ds) {
    ds->setHorDivergence(parser.parseHorDiv());
    ds->setVerDivergence(parser.parseVerDiv());
}

void setDefaultPosition(xml::Parser parser, DesignSource* ds) {
    ds->setSourceDepth(parser.parseSourceDepth());
    ds->setSourceHeight(parser.parseSourceHeight());
    ds->setSourceWidth(parser.parseSourceWidth());
}

void setStokes(xml::Parser parser, DesignSource* ds) {
    ds->setStokeslin0(parser.parseLinearPol0());
    ds->setStokeslin45(parser.parseLinearPol45());
    ds->setStokescirc(parser.parseCircularPol());
}

void setPointSource(xml::Parser parser, DesignSource* ds) {
    setAllMandatory(parser, ds);
    setStokes(parser, ds);
    setDefaultEnergy(parser, ds);

    setDefaultPosition(parser, ds);
    ds->setWidthDist(parser.parseSourceWidthDistribution());
    ds->setHeightDist(parser.parseSourceHeightDistribution());

    setDefaultOrientation(parser, ds);
    ds->setHorDist(parser.parseHorDivDistribution());
    ds->setVerDist(parser.parseVerDivDistribution());
}

void setMatrixSource(xml::Parser parser, DesignSource* ds) {
    setAllMandatory(parser, ds);
    setStokes(parser, ds);
    setDefaultEnergy(parser, ds);
    setDefaultPosition(parser, ds);
    setDefaultOrientation(parser, ds);
}

void setDipoleSource(xml::Parser parser, DesignSource* ds) {
    setAllMandatory(parser, ds);

    ds->setEnergySpreadType(parser.parseEnergySpreadType());
    ds->setPhotonFlux(parser.parsePhotonFlux());
    ds->setElectronEnergyOrientation(parser.parseElectronEnergyOrientation());
    ds->setElectronEnergy(parser.parseElectronEnergy());
    ds->setEnergySpread(parser.parseEnergySpread());
    ds->setBendingRadius(parser.parseBendingRadiusDouble());
    ds->setSourceHeight(parser.parseSourceHeight());
    ds->setSourceWidth(parser.parseSourceWidth());
    ds->setVerEBeamDivergence(parser.parseVerEbeamDivergence());
    ds->setEnergy(parser.parsePhotonEnergy());
    ds->setEnergySpreadUnit(parser.parseEnergySpreadUnit());
    ds->setEnergyDistributionType(parser.parseEnergyDistributionType());
    ds->setHorDivergence(parser.parseHorDiv());
}

void setPixelSource(xml::Parser parser, DesignSource* ds) {
    setAllMandatory(parser, ds);
    setStokes(parser, ds);
    setDefaultEnergy(parser, ds);
    setDefaultPosition(parser, ds);
    setDefaultOrientation(parser, ds);
}

void setCircleSource(xml::Parser parser, DesignSource* ds) {
    setAllMandatory(parser, ds);
    setStokes(parser, ds);
    setDefaultEnergy(parser, ds);
    setDefaultPosition(parser, ds);

    ds->setNumOfCircles(parser.parseNumOfEquidistantCircles());
    ds->setMaxOpeningAngle(parser.parseMaxOpeningAngle());
    ds->setMinOpeningAngle(parser.parseMinOpeningAngle());
    ds->setDeltaOpeningAngle(parser.parseDeltaOpeningAngle());
}

void setSimpleUndulatorSource(xml::Parser parser, DesignSource* ds) {
    setAllMandatory(parser, ds);
    setStokes(parser, ds);
    setDefaultEnergy(parser, ds);

    ds->setSourceDepth(parser.parseSourceDepth());

    ds->setSigmaType(parser.parseSigmaType());

    ds->setUndulatorLength(parser.parseUndulatorLength());
    ds->setElectronSigmaX(parser.parseElectronSigmaX());
    ds->setElectronSigmaXs(parser.parseElectronSigmaXs());
    ds->setElectronSigmaY(parser.parseElectronSigmaY());
    ds->setElectronSigmaYs(parser.parseElectronSigmaYs());
}

}  // namespace RAYX