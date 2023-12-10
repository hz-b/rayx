#pragma once

#include "TypedTable.h"
#include "TypedVariant.h"

namespace RAYX {

struct DesignMatrixSource;

struct DesignLightSourceType : public TypedVariant<
    Case<DesignMatrixSource, MatrixSourceStr>
> {};

struct DesignMatrixSource : public TypedTable<
    Field<double, LinearPol0Str>,
    Field<double, LinearPol45Str>,
    Field<double, CircularPolStr>,
    Field<double, SourceDepthStr>
> {};

}
