// This file is intended to bridge the gap between the GLSL and the c++ languages by defining a common API for both languages.
// Every header file in the shader needs to include Common.h (first!) in order to be safely used by C++ and GLSL.

#ifndef ADAPT_H
#define ADAPT_H

#ifdef RAYX_CORE
#include <alpaka/core/Common.hpp>
#define RAYX_FUNC ALPAKA_FN_ACC
#else
#define RAYX_FUNC
#endif

// glm definitions need to be directly accessible in c++.
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
using glm::dvec2;
using glm::dvec3;
using glm::dvec4;
using glm::dmat3;
using glm::dmat4;
using uint = unsigned int;
// These functions are per default provided by GLSL.
using glm::length;
using glm::mod;
using glm::sign;

// throws an error, and termiantes the program
// TODO(Sven): rethink error handling. just instantly terminate with RAYX_ERR or use recordFinalEvent?
// #define _throw(string) recordFinalEvent(_ray, ETYPE_FATAL_ERROR)
#include "Debug/Debug.h"
// #define _throw(string) RAYX_ERR << string
#define _throw(string) assert(false)
// #endif

// This way I don't have to worry about namespaces..
using namespace RAYX;

#endif
