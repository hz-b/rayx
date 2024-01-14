#pragma once

#include <filesystem>
#include <vector>

#include "Core.h"

namespace RAYX {

// TODO remove this function. It's outdated and unsafe.
uint32_t* readFile(uint32_t& length, const char* filename);

std::vector<uint8_t> RAYX_API readFile(const std::string& filename, const uint32_t count = 0);

void RAYX_API writeFile(const std::vector<uint8_t>& data, const std::string& filename, const uint32_t count = 0);

}  // namespace RAYX
