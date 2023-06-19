#pragma once

#include <filesystem>

#include "Beamline/Beamline.h"
#include "Core.h"
namespace RAYX {

struct RAYX_API RenderData {
    std::string name;
    glm::vec4 position;
    glm::mat4 orientation;
    Cutout cutout;
};
using RenderDataVec = std::vector<RenderData>;

RAYX_API RenderDataVec getRenderData(const std::filesystem::path& filename);
RAYX_API Beamline importBeamline(const std::filesystem::path&);

}  // namespace RAYX