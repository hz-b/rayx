#pragma once

#include <filesystem>

#include "RaySoA.h"

namespace RAYX {

RAYX_API RaySoA readCsvRaySoA(const std::filesystem::path& filename);
RAYX_API BundleHistory readCsvBundleHistory(const std::filesystem::path& filename);

RAYX_API void writeCsvRaySoA(const std::filesystem::path& filename, const RaySoA& rays, const RayAttrFlag attr = RayAttrFlag::All);
RAYX_API void writeCsvBundleHistory(const std::filesystem::path& filename, const BundleHistory& bundle, const RayAttrFlag attr = RayAttrFlag::All);

}  // namespace RAYX
