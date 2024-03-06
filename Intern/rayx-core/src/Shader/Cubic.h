#ifndef CUBIC_H
#define CUBIC_H

#include "Common.h"

//TODO: doku
RAYX_FUNC dvec3 RAYX_API cubicPosition(Ray r, double alpha);

RAYX_FUNC dvec3 RAYX_API cubicDirection(Ray r, double alpha);

#endif
