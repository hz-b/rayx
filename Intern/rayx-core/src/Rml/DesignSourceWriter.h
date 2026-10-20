#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Debug/Debug.h"
#include "Element/Cutout.h"

namespace rayx {

namespace {
EnergySpreadUnit parseEnergySpreadUnitOrDefault(xml::Parser parser) {
    int energySpreadUnit = 0;
    if (!xml::paramInt(parser.node, "energySpreadUnit", &energySpreadUnit)) { return EnergySpreadUnit::EU_eV; }

    return static_cast<EnergySpreadUnit>(energySpreadUnit);
}
}  // unnamed namespace

void setAllMandatory(xml::Parser parser, DesignSource* ds) {
    ds->setName(parser.name());
    ds->setType(parser.type());
    ds->setNumberOfRays(parser.parseNumberRays());
    ds->setOrientation(parser.parseOrientation());
    ds->setPosition(parser.parsePosition());
}

void setDefaultEnergy(xml::Parser parser, DesignSource* ds) {
    ds->setEnergyDistributionType(parser.parseEnergyDistributionType());

    if (ds->getEnergyDistributionType() == EnergyDistributionType::File) {
        // RAY-UI treats the value-based spread parameters as irrelevant in file mode.
        ds->setEnergyDistributionFile(parser.parseEnergyDistributionFile().generic_string());
        return;
    }

    int numberOfEnergies = -1;
    ds->setEnergySpreadType(parser.decodeRayUiEnergySpreadType(&numberOfEnergies));
    ds->setEnergy(parser.parsePhotonEnergy());
    ds->setEnergySpread(parser.parseEnergySpread());
    ds->setEnergySpreadUnit(parseEnergySpreadUnitOrDefault(parser));
    if (numberOfEnergies > 0) ds->setNumberOfSeparateEnergies(numberOfEnergies);
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

    // The dipole derives its spectrum from the electron beam; RAY-UI's spread type does not apply.
    int rayUiSpreadType = 0;
    if (xml::paramInt(parser.node, "energySpreadType", &rayUiSpreadType) && rayUiSpreadType != 0) {
        RAYX_WARN << "dipole source \"" << parser.name() << "\": energySpreadType=" << rayUiSpreadType
                  << " does not apply to dipole sources and is ignored; the spectrum is derived from the electron beam";
    }
    ds->setEnergySpreadType(SpreadType::HardEdge);
    ds->setPhotonFlux(parser.parsePhotonFlux());
    ds->setElectronEnergyOrientation(parser.parseElectronEnergyOrientation());
    ds->setElectronEnergy(parser.parseElectronEnergy());
    ds->setEnergySpread(parser.parseEnergySpread());
    ds->setBendingRadius(parser.parseBendingRadiusDouble());
    ds->setSourceHeight(parser.parseSourceHeight());
    ds->setSourceWidth(parser.parseSourceWidth());
    ds->setVerEBeamDivergence(parser.parseVerEbeamDivergence());
    ds->setEnergy(parser.parsePhotonEnergy());
    ds->setEnergySpreadUnit(parseEnergySpreadUnitOrDefault(parser));
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

}  // namespace rayx
