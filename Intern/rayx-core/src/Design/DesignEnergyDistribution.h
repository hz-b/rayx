#pragma once

#include "TypedVariant.h"
#include "TypedTable.h"
#include "Data/DatFile.h"

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
> {};

struct DesignHardEdge : public TypedTable<
    Field<double, CenterEnergyStr>,
    Field<double, EnergySpreadStr>
> {};

struct DesignSoftEdge : public TypedTable<
    Field<double, CenterEnergyStr>,
    Field<double, SigmaStr>
> {};

struct DesignSeparateEnergies : public TypedTable<
    Field<double, CenterEnergyStr>,
    Field<double, EnergySpreadStr>,
    Field<int, NumberOfEnergiesStr>
> {};

}
