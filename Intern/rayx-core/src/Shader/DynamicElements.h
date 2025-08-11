#pragma once

#include "Core.h"
#include "InvocationState.h"

namespace RAYX {

// @brief: Dynamic ray tracing: check which ray hits which element first
// in this function we need to make sure that rayData ALWAYS remains in GLOBAL coordinates (it can be changed in a function but needs to be changed
// back before the function returns to this function)
RAYX_FN_ACC void dynamicElements(const int gid, const ConstState& constState, MutableState& mutableState);

}  // namespace RAYX
