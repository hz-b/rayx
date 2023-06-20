// This file is inteded to bridge the gap between the GLSL and the c++ languages by defining a common API for both languages.

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
using dmat4 = glm::dmat4;
using dvec4 = glm::dvec4;
using dvec3 = glm::dvec3;
#endif

// RAYX_API can be ignored in glsl.
#ifdef GLSL
#define RAYX_API
#endif

// define INOUT, OUT, ALLOW_UNUSED
#ifndef GLSL
#define INOUT(x) x&
#define OUT(x) x&
#define ALLOW_UNUSED [[maybe_unused]]
#else
#define INOUT(x) inout x
#define OUT(x) out x
#define ALLOW_UNUSED
#endif

// define SHADER_ARRAY
#ifdef GLSL
#define SHADER_ARRAY(T, ident, binding_id, bufname) \
    layout(std430, binding = binding_id) buffer bufname { T ident[]; }
#else
#define SHADER_ARRAY(T, ident, binding_id, bufname) ShaderArray<T> ident
#endif

#endif
