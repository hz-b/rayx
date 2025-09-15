#pragma once

#include <filesystem>

#include "Rays.h"

namespace RAYX {

#ifndef NO_H5
RAYX_API Rays readH5Rays(const std::filesystem::path& filepath, const RayAttrMask attr = RayAttrMask::All);
RAYX_API std::vector<std::string> readH5ObjectNames(const std::filesystem::path& filepath);

RAYX_API void writeH5Rays(const std::filesystem::path& filepath, const std::vector<std::string>& object_names,
                          const Rays& rays, const RayAttrMask attr = RayAttrMask::All);
#endif

}  // namespace RAYX
