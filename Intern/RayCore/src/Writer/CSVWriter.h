#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Tracer/Ray.h"

void RAYX_API writeCSV(const std::vector<RAYX::Ray>&, std::string filename);

std::vector<RAYX::Ray> loadCSV(std::string filename);
