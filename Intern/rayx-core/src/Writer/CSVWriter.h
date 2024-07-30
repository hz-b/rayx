#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

namespace RAYX {

void RAYX_API writeCSV(const BundleHistory&, const std::string& filename, const Format& format, int startEventID = 0);

// loadCSV only works for csv files created using FULL_FORMAT.
BundleHistory RAYX_API loadCSV(const std::string& filename);

}  // namespace RAYX
