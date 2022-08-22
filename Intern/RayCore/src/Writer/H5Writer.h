#ifndef CI  // highfive doesn't work in CI

#pragma once

#include "Tracer/RayList.h"

void writeH5(RAYX::RayList&, std::string filename);

#endif
