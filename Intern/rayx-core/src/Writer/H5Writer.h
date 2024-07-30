#pragma once

#include <string>
#include <vector>

#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

#ifndef NO_H5

namespace RAYX {

RAYX_API void writeH5(const BundleHistory&, const std::string& filename, const Format& format, std::vector<std::string> elementNames,
                      int startEventID);
RAYX_API BundleHistory raysFromH5(const std::string& filename, const Format& format, unsigned int* startEventID = nullptr);

}  // namespace RAYX

#endif
