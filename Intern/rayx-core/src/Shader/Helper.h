#pragma once

#include "Common.h"
#include "Ray.h"
#include "InvocationState.h"

namespace RAYX {

RAYX_FN_ACC void init(Inv& inv);
RAYX_FN_ACC uint64_t rayId(Inv& inv);
RAYX_FN_ACC uint output_index(uint i, Inv& inv);
RAYX_FN_ACC void recordEvent(Ray r, double w, Inv& inv);
RAYX_FN_ACC void recordFinalEvent(Ray r, double w, Inv& inv);

} // namespace RAYX
