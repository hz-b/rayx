#pragma once

#include <filesystem>

#include "RaySoA.h"

namespace RAYX {

#ifndef NO_H5
RAYX_API RaySoA readH5RaySoA(const std::filesystem::path& filepath, const RayAttrFlag attr = RayAttrFlag::All);
RAYX_API BundleHistory readH5BundleHistory(const std::filesystem::path& filepath);
RAYX_API std::vector<std::string> readH5SourceNames(const std::filesystem::path& filepath);
RAYX_API std::vector<std::string> readH5ElementNames(const std::filesystem::path& filepath);

RAYX_API void writeH5RaySoA(const std::filesystem::path& filepath, const std::vector<std::string>& source_names, const std::vector<std::string>& element_names, const RaySoA& rays,
                            const RayAttrFlag attr = RayAttrFlag::All);
RAYX_API void writeH5BundleHistory(const std::filesystem::path& filepath, const std::vector<std::string>& source_names, const std::vector<std::string>& element_names, const BundleHistory& bundle,
                                   const RayAttrFlag attr = RayAttrFlag::All);
#endif

}  // namespace RAYX
