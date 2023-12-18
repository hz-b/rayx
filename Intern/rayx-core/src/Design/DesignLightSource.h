#pragma once

#include "TypedTable.h"
#include "DesignElement.h"
#include "Strings.h"
#include "DesignEnergyDistribution.h"
#include "DesignLightSourceType.h"
#include "DesignTransform.h"
#include "DesignMisalignment.h"

namespace RAYX {

struct DesignLightSource : public TypedTable<
    Field<std::string, NameStr>,
    Field<DesignEnergyDistribution, EnergyDistributionStr>,
    Field<DesignLightSourceType, LightSourceTypeStr>,
    Field<int, NumberOfRaysStr>,
    Field<double, SourceHeightStr>,
    Field<double, SourceWidthStr>,
    Field<double, HorDivergenceStr>,
    Field<double, VerDivergenceStr>,
    Field<DesignTransform, TransformStr>,
    Field<DesignMisalignment, MisalignmentStr>
> {
    std::vector<Ray> getRays() const;
};

}
