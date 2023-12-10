#pragma once

#include "TypedVariant.h"
#include "TypedTable.h"

namespace RAYX {

struct TransformMatrix : public TypedTable<
    Field<std::vector<double>, MatrixStr>
>{ };

// TODO
struct DesignTransform : public TypedVariant<
    Case<TransformMatrix, ByMatrixStr>
> {};

}
