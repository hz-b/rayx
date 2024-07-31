#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

RAYX_API void writeH5(const RAYX::BundleHistory&, const std::string& filename, const Format& format, std::vector<std::string> elementNames,
                      int startEventID);
RAYX_API RAYX::BundleHistory raysFromH5(const std::string& filename, const Format& format, std::unique_ptr<uint32_t> startEventID = nullptr);
