#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

void RAYX_API writeCSV(const RAYX::BundleHistory&, const std::string& filename, const Format& format);

// loadCSV only works for csv files created using FULL_FORMAT.
RAYX::BundleHistory RAYX_API loadCSV(const std::string& filename);
