#pragma once

#include <filesystem>

#include "Beamline/Beamline.h"
#include "Core.h"
namespace RAYX {

RAYX_API Beamline importBeamline(const std::filesystem::path&);

}  // namespace RAYX