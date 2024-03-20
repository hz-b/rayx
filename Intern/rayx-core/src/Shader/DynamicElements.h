#ifndef DYNAMIC_ELEMENTS_H
#define DYNAMIC_ELEMENTS_H

#include "Common.h"
#include "InvocationState.h"

// @brief: Dynamic ray tracing: check which ray hits which element first
// in this function we need to make sure that rayData ALWAYS remains in GLOBAL coordinates (it can be changed in a function but needs to be changed
// back before the function returns to this function)
RAYX_FUNC void dynamicElements(int gid, Inv inv);

#endif
