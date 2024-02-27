#ifndef HELPER_H
#define HELPER_H

#include "Common.h"
#include "Ray.h"
#include "InvocationState.h"

void init(Inv& inv);
uint64_t rayId(Inv& inv);
uint output_index(uint i, Inv& inv);
void recordEvent(Ray r, double w, Inv& inv);
void recordFinalEvent(Ray r, double w, Inv& inv);

#endif
