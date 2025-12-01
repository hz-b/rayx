#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "Core.h"

namespace rayx {

// The readFile functions return {} if an error occured while opening the file.
// We prefered this over an exception, as you typically forget to handle them.
std::optional<std::vector<uint8_t>> RAYX_API readFile(const std::string& filename, const uint32_t count = 0);

// The alignment of 32-bits is necessary for SPIR-V shader code.
// See https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkShaderModuleCreateInfo.html
std::optional<std::vector<uint32_t>> readFileAlign32(const std::string& filename, const uint32_t count = 0);

void RAYX_API writeFile(const std::vector<uint8_t>& data, const std::string& filename, const uint32_t count = 0);

}  // namespace rayx
