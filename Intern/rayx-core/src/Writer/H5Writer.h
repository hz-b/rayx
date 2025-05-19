#pragma once

#include <filesystem>

#include "RaySoA.h"

namespace RAYX {

// TODO: move this function to writer.h
RayAttrFlag formatStringToRayAttrFlag(const std::string& format);

RAYX_API RaySoA readH5RaySoA(const std::filesystem::path& filename, const RayAttrFlagType attr = AllRayAttr);
RAYX_API BundleHistory readH5BundleHistory(const std::filesystem::path& filename);

RAYX_API void writeH5RaySoA(const std::filesystem::path& filename, const RaySoA& rays, const RayAttrFlagType attr = AllRayAttr);
RAYX_API void writeH5BundleHistory(const std::filesystem::path& filename, const BundleHistory& bundle, const RayAttrFlagType attr = AllRayAttr);

}  // namespace RAYX
