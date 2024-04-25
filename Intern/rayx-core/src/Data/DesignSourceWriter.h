#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shader/Cutout.h"

namespace RAYX {

void setAllMandatory(xml::Parser parser, DesignSource* ds) {
    ds->setName(parser.name());
    ds->setType(parser.type());
    ds->setMisalignment(parser.parseMisalignment());
    ds->setNumberOfRays(parser.parseNumberRays());
    ds->setWorldOrientation(parser.parseOrientation());
    ds->setWorldPosition(parser.parsePosition());

    ds->setSeperateEnergies(1);
}

void setDefaultEnergy(xml::Parser parser, DesignSource* ds) {
    ds->setEnergy(parser.parsePhotonEnergy());
    ds->setEnergyDistributionFile(parser.parseEnergyDistributionFile().generic_string());
    ds->setEnergyDistributionType(parser.parseEnergyDistributionType());
    ds->setEnergySpread(parser.parseEnergySpread());
    ds->setEnergySpreadType(parser.parseEnergySpreadType());
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
    ds->setElectronEnergyOriantation(parser.parseElectronEnergyOrientation());
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