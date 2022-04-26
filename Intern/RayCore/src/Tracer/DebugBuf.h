#pragma once
#pragma pack(16)

#include <Core.h>

#include <glm.hpp>

//TODO: IF extending Debug buffer, use this instead.
namespace RAYX {
struct RAYX_API DebugBuf {
    glm::dmat4x4 _dmat;
};
}  // namespace RAYX