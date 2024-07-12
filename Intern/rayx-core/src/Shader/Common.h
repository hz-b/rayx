#pragma once

#include "Core.h"

#ifdef RAYX_BUILD_DLL
#include <alpaka/core/Common.hpp>
#define RAYX_FN_ACC ALPAKA_FN_ACC
#else
#define RAYX_FN_ACC
#endif

#include <glm.h>

namespace RAYX {

using glm::dvec2;
using glm::dvec3;
using glm::dvec4;
using glm::dmat3;
using glm::dmat4;

using glm::length;
using glm::mod;
using glm::sign;
using glm::abs;

using uint = unsigned int;

} // namespace RAYX
