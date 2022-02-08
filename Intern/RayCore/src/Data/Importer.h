#pragma once

#include <memory>

#include "Core.h"
#include "Model/Beamline/Beamline.h"

namespace RAYX {

Beamline importBeamline(const char* filename);

}  // namespace RAYX