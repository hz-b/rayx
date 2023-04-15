#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Tracer/Ray.h"
#include "Tracer/Tracer.h"

void RAYX_API writeCSV(const RAYX::Rays&, std::string filename);

RAYX::Rays RAYX_API loadCSV(std::string filename);
