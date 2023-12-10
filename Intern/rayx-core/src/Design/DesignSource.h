#pragma once

#include "TypedTable.h"
#include "DesignElement.h"
#include "Strings.h"
#include "DesignEnergyDistribution.h"
#include "DesignSourceType.h"
#include "DesignTransform.h"
#include "DesignMisalignment.h"

namespace RAYX {

struct DesignSource : public TypedTable<
    Field<std::string, NameStr>,
    Field<DesignEnergyDistribution, EnergyDistributionStr>,
    Field<DesignSourceType, SourceTypeStr>,
    Field<int, NumberOfRaysStr>,
    Field<double, SourceHeightStr>,
    Field<double, SourceWidthStr>,
    Field<double, HorDivergenceStr>,
    Field<double, VerDivergenceStr>,
    Field<DesignTransform, TransformStr>,
    Field<DesignMisalignment, MisalignmentStr>
> {};

}
