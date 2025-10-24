#pragma once

#include <filesystem>

#include "Beamline/Beamline.h"
#include "Core.h"

namespace rayx {

// This is the central point where the beamline is loaded from an RML file.
// Any future plans for other formats, will *not* use this Importer. It's intended for XML.
RAYX_API Beamline importBeamline(const std::filesystem::path&);

}  // namespace rayx
