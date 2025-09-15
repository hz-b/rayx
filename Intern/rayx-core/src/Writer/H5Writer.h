#pragma once

#include <filesystem>

#include "Rays.h"

namespace RAYX {

#ifndef NO_H5
RAYX_API Rays readH5Rays(const std::filesystem::path& filepath, const RayAttrMask attr = RayAttrMask::All);
RAYX_API BundleHistory readH5BundleHistory(const std::filesystem::path& filepath);
RAYX_API std::vector<std::string> readH5SourceNames(const std::filesystem::path& filepath);
RAYX_API std::vector<std::string> readH5ElementNames(const std::filesystem::path& filepath);

RAYX_API void writeH5Rays(const std::filesystem::path& filepath, const std::vector<std::string>& source_names,
                          const std::vector<std::string>& element_names, const Rays& rays, const RayAttrMask attr = RayAttrMask::All);
RAYX_API void writeH5BundleHistory(const std::filesystem::path& filepath, const std::vector<std::string>& source_names,
                                   const std::vector<std::string>& element_names, const BundleHistory& bundle,
                                   const RayAttrMask attr = RayAttrMask::All);
#endif

}  // namespace RAYX
