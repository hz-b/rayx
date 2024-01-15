#pragma once

#include <filesystem>
#include <vector>

#include "Core.h"

namespace RAYX {

// The alignment of 32-bits is necessary for SPIR-V shader code.
// See https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkShaderModuleCreateInfo.html
uint32_t* readFileAlign32(uint32_t& length, const char* filename);

std::vector<uint8_t> RAYX_API readFile(const std::string& filename, const uint32_t count = 0);

void RAYX_API writeFile(const std::vector<uint8_t>& data, const std::string& filename, const uint32_t count = 0);

}  // namespace RAYX
