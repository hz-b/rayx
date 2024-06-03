// This file is intended to bridge the gap between the GLSL and the c++ languages by defining a common API for both languages.
// Every header file in the shader needs to include Common.h (first!) in order to be safely used by C++ and GLSL.

#pragma once

#include "Core.h"

#ifdef RAYX_BUILD_DLL
#include <alpaka/core/Common.hpp>
#define RAYX_FUNC ALPAKA_FN_ACC
#else
#define RAYX_FUNC
#endif

// glm definitions need to be directly accessible in c++.
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>

namespace RAYX {

using glm::dvec2;
using glm::dvec3;
using glm::dvec4;
using glm::dmat3;
using glm::dmat4;

using glm::length;
using glm::mod;
using glm::sign;

using uint = unsigned int;

} // namespace RAYX
