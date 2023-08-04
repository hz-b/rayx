// This file is intended to bridge the gap between the GLSL and the c++ languages by defining a common API for both languages.

#ifndef ADAPT_H
#define ADAPT_H

// INLINE can be ignored in glsl.
#ifdef GLSL
#define INLINE
#else
#define INLINE inline
#endif

// glm definitions need to be directly accessible in c++.
#ifndef GLSL

#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
using dvec2 = glm::dvec2;
using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using dmat3 = glm::dmat3;
using dmat4 = glm::dmat4;
#endif

// RAYX_API can be ignored in glsl.
#ifdef GLSL
#define RAYX_API
#endif

// define RAYX_INOUT, RAYX_OUT, ALLOW_UNUSED
#ifndef GLSL
#define RAYX_INOUT(x) x&
#define RAYX_OUT(x) x&
#define ALLOW_UNUSED [[maybe_unused]]
#else
#define RAYX_INOUT(x) inout x
#define RAYX_OUT(x) out x
#define ALLOW_UNUSED
#endif

// define SHADER_ARRAY
#ifdef GLSL
#define SHADER_ARRAY(T, ident, binding_id, bufname) \
    layout(std430, binding = binding_id) buffer bufname { T ident[]; }
#else

#define SHADER_ARRAY(T, ident, binding_id, bufname) ShaderArray<T> RAYX_API ident

#ifdef GLSL
#define SHADER_PRINT(msg) debugPrintfEXT(msg)
#define SHADER_PRINT1(msg, arg1) debugPrintfEXT(msg, arg1)
#define SHADER_PRINT2(msg, arg1, arg2) debugPrintfEXT(msg, arg1, arg2)
#define SHADER_PRINT3(msg, arg1, arg2, arg3) debugPrintfEXT(msg, arg1, arg2, arg3)
#else
#define SHADER_PRINT(msg) printf(msg)
#define SHADER_PRINT1(msg, arg1) printf(msg, arg1)
#define SHADER_PRINT2(msg, arg1, arg2) printf(msg, arg1, arg2)
#define SHADER_PRINT3(msg, arg1, arg2, arg3) printf(msg, arg1, arg2, arg3)
#endif

// this type intends to mimic the GLSL type T[], this is used for layouts.
template <typename T>
struct ShaderArray {
  public:
    std::vector<T> data;

    inline int length() { return data.size(); }
    inline T& operator[](int i) { return data[i]; }
};

#endif

// These functions are per default provided by GLSL.
#ifndef GLSL
inline double length(dvec2 v) { return sqrt(v.x * v.x + v.y * v.y); }
inline double mod(double x, double y) { return glm::mod(x, y); }
inline double sign(double x) { return glm::sign(x); }
#endif

#endif
