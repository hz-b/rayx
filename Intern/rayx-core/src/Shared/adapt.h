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
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3;
using mat4 = glm::mat4;
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
inline float length(vec2 v) { return sqrt(v.x * v.x + v.y * v.y); }
inline float mod(float x, float y) { return glm::mod(x, y); }
inline float sign(float x) { return glm::sign(x); }
#endif

#endif
