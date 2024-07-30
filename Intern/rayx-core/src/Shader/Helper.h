#pragma once

#include "Core.h"
#include "InvocationState.h"
#include "Ray.h"

namespace RAYX {

RAYX_FN_ACC void init(InvState& inv);
RAYX_FN_ACC uint64_t rayId(InvState& inv);
RAYX_FN_ACC uint output_index(uint i, InvState& inv);
RAYX_FN_ACC void recordEvent(Ray r, double w, InvState& inv);
RAYX_FN_ACC void recordFinalEvent(Ray r, double w, InvState& inv);

}  // namespace RAYX
