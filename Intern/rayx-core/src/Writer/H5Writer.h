#ifndef NO_H5

#pragma once

#include <string>
#include <vector>

#include "Shared/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

RAYX_API void writeH5(const RAYX::BundleHistory&, const std::string& filename, const Format& format, std::vector<std::string> elementNames);
RAYX_API RAYX::BundleHistory raysFromH5(const std::string& filename, const Format& format);

#endif
