// This file is intended to bridge the gap between the GLSL and the c++ languages by defining a common API for both languages.
// Every header file in the shader needs to include Common.h (first!) in order to be safely used by C++ and GLSL.

#ifndef ADAPT_H
#define ADAPT_H

#define INLINE inline
#define RAYX_INOUT(x) x&
#define RAYX_OUT(x) x&
#define ALLOW_UNUSED [[maybe_unused]]

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

// define SHADER_ARRAY
#define SHADER_ARRAY(T, ident, binding_id, bufname) ShaderArray<T> RAYX_API ident

// this type intends to mimic the GLSL type T[], this is used for layouts.
template <typename T>
struct ShaderArray {
  public:
    std::vector<T> data;

    inline int length() { return data.size(); }
    inline T& operator[](int i) { return data[i]; }
};

// throws an error, and termiantes the program
// TODO(Sven): rethink error handling. just instantly terminate with RAYX_ERR or use recordFinalEvent?
// #ifdef GLSL
// #define _throw(string) recordFinalEvent(_ray, ETYPE_FATAL_ERROR)
// #else
#include "Debug/Debug.h"
#define _throw(string) RAYX_ERR << string
// #endif

// This way I don't have to worry about namespaces..
using namespace RAYX;

#endif
