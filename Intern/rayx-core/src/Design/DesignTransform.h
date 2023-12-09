#pragma once

#include "TypedVariant.h"

namespace RAYX {

// TODO
struct DesignTransform : public TypedVariant<
    Case<std::vector<double>, ByMatrixStr>
> {};

}
