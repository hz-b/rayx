#pragma once

#include <filesystem>

#include "RaySoA.h"

namespace RAYX {

#ifndef NO_H5
RAYX_API RaySoA readH5RaySoA(const std::filesystem::path& filename, const RayAttrFlag attr = RayAttrFlag::All);
RAYX_API BundleHistory readH5BundleHistory(const std::filesystem::path& filename);

RAYX_API void writeH5RaySoA(const std::filesystem::path& filename, const RaySoA& rays, const RayAttrFlag attr = RayAttrFlag::All);
RAYX_API void writeH5BundleHistory(const std::filesystem::path& filename, const BundleHistory& bundle, const RayAttrFlag attr = RayAttrFlag::All);
#endif

}  // namespace RAYX
