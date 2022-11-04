#ifndef CI  // highfive doesn't work in CI

#pragma once

#include <string>
#include <vector>

#include "Tracer/Ray.h"

void RAYX_API writeH5(const std::vector<RAYX::Ray>&, std::string filename);

#endif
