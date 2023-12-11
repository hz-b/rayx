#pragma once

#include "TypedVariant.h"
#include "TypedTable.h"

namespace RAYX {

struct DesignHardEdge;
struct DesignSoftEdge;
struct DesignSeparateEnergies;
struct DesignDatFile;

struct DesignEnergyDistribution : public TypedVariant<
    Case<DesignHardEdge, HardEdgeStr>,
    Case<DesignSoftEdge, SoftEdgeStr>,
    Case<DesignSeparateEnergies, SeparateEnergiesStr>,
    Case<DesignDatFile, DatFileStr>
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

// TODO how do I handle dat file? would be cool, if it's checked 'fail fast'.
struct DesignDatFile : public TypedTable<> {};

}
