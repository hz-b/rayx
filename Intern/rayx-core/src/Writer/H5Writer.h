#ifndef NO_H5

#pragma once

#include <string>
#include <vector>

#include "Tracer/Ray.h"
#include "Tracer/Tracer.h"

void RAYX_API writeH5(const RAYX::Rays&, std::string filename, std::vector<std::string> elementNames);

#endif
