#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"

namespace RAYX {

void RAYX_API writeCsv(const std::filesystem::path& filepath, const Rays& rays);

// TODO: enable
// void RAYX_API writeCsv(const RAYX::BundleHistory&, const std::string& filename);
//
// RAYX::BundleHistory RAYX_API loadCsv(const std::string& filename);

}  // namespace RAYX
