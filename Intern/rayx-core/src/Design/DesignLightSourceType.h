#pragma once

#include "TypedTable.h"
#include "TypedVariant.h"
#include "Strings.h"

namespace RAYX {

struct DesignMatrixSource;
struct DesignLightSource;

struct DesignLightSourceType : public TypedVariant<
    Case<DesignMatrixSource, MatrixSourceStr>
> {
    std::vector<Ray> getRays(const DesignLightSource&) const;
};

struct DesignMatrixSource : public TypedTable<
    Field<double, LinearPol0Str>,
    Field<double, LinearPol45Str>,
    Field<double, CircularPolStr>,
    Field<double, SourceDepthStr>
> {
    std::vector<Ray> getRays(const DesignLightSource&) const;
};

}
