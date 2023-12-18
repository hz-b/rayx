#pragma once

#include "TypedVariant.h"
#include "TypedTable.h"
#include "Data/DatFile.h"
#include "Random.h"

namespace RAYX {

struct DesignHardEdge;
struct DesignSoftEdge;
struct DesignSeparateEnergies;
struct DesignDatFile;

struct DesignEnergyDistribution : public TypedVariant<
    Case<DesignHardEdge, HardEdgeStr>,
    Case<DesignSoftEdge, SoftEdgeStr>,
    Case<DesignSeparateEnergies, SeparateEnergiesStr>,
    Case<DatFile, DatFileStr>
> {
    double selectEnergy() const;
};

struct DesignHardEdge : public TypedTable<
    Field<double, CenterEnergyStr>,
    Field<double, EnergySpreadStr>
> {
    double selectEnergy() const;
};

struct DesignSoftEdge : public TypedTable<
    Field<double, CenterEnergyStr>,
    Field<double, SigmaStr>
> {
    double selectEnergy() const;
};

struct DesignSeparateEnergies : public TypedTable<
    Field<double, CenterEnergyStr>,
    Field<double, EnergySpreadStr>,
    Field<int, NumberOfEnergiesStr>
> {
    double selectEnergy() const;
};

}
