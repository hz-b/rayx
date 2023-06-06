#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Tracer/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

void RAYX_API writeCSV(const RAYX::Rays&, std::string filename, const Format& format);

// loadCSV only works for csv files created using FULL_FORMAT.
RAYX::Rays RAYX_API loadCSV(std::string filename);
