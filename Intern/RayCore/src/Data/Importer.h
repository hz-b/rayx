#pragma once

#include <filesystem>
#include <memory>

#include "Core.h"
#include "Model/Beamline/Beamline.h"

namespace RAYX {

RAYX_API Beamline importBeamline(const std::filesystem::path&);

}  // namespace RAYX