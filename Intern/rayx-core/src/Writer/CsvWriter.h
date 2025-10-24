#pragma once

#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"

namespace rayx {

void RAYX_API writeCsv(const std::filesystem::path& filepath, const Rays& rays);
Rays RAYX_API readCsv(const std::filesystem::path& filepath);

}  // namespace rayx
