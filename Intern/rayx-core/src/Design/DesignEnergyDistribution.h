#pragma once

#include "TypedVariant.h"

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

struct DesignHardEdge : public TypedTable<> {};
struct DesignSoftEdge : public TypedTable<> {};
struct DesignSeparateEnergies : public TypedTable<> {};
struct DesignDatFile : public TypedTable<> {};

}
