#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

namespace RAYX {

void RAYX_API writeCsv(const RAYX::BundleHistory&, const std::string& filename, const Format& format);

RAYX::BundleHistory RAYX_API loadCsv(const std::string& filename, const bool rayUiCompatible = false);

}  // namespace RAYX
