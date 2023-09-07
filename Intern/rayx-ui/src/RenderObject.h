#pragma once

#include "Data/Importer.h"
#include "Shared/Cutout.h"

struct RenderObject {
    std::string name;
    int type;
    double surface[16];
    glm::vec4 position;
    glm::mat4 orientation;
    Cutout cutout;
};
using RenderObjectVec = std::vector<RenderObject>;

RenderObjectVec getRenderData(const std::filesystem::path& filename);