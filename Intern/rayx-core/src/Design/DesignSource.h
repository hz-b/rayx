#pragma once

#include "TypedTable.h"
#include "DesignElement.h"
#include "Strings.h"
#include "DesignEnergyDistribution.h"
#include "DesignSourceType.h"

namespace RAYX {

struct DesignSource : public TypedTable<
    Field<std::string, NameStr>,
    Field<DesignEnergyDistribution, EnergyDistributionStr>,
    Field<DesignSourceType, SourceTypeStr>,
    Field<int, NumberOfRaysStr>
> {};

}
