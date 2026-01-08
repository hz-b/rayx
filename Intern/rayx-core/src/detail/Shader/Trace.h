#pragma once

#include "Core.h"
#include "InvocationState.h"

namespace rayx {

RAYX_FN_ACC void traceSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState);
RAYX_FN_ACC void traceNonSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState);

}  // namespace rayx
