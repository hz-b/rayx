#ifndef NO_H5

#pragma once

#include <string>
#include <vector>

#include "Shared/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

RAYX_API void writeH5(const RAYX::Rays&, std::string filename, const Format& format, std::vector<std::string> elementNames);
RAYX_API void readH5(RAYX::Rays& rays, std::string filename, const Format& format);
RAYX::Rays fromDoubles(const std::vector<double>& doubles, const Format& format);

#endif
