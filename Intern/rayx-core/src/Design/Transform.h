#pragma once

#include "Translation.h"
#include "Rotation.h"

namespace rayx::design {

struct Transform {
    Translation translation;
    Rotation rotation;
};

}  // namespace rayx::design
