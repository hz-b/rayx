#ifndef HELPER_H
#define HELPER_H

#include "Adapt.h"
#include "Ray.h"

void init();
uint64_t rayId();
uint output_index(uint i);
void recordEvent(Ray r, double w);
void recordFinalEvent(Ray r, double w);

#endif
