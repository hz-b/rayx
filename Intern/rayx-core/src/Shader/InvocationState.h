#ifndef INVOCATION_STATE_H
#define INVOCATION_STATE_H

#include "Adapt.h"
#include "Ray.h"

// This file groups all global variables from the shader to one spot.

// Useful for GPU Tracing
struct PushConstants {  // TODO(Jannis): PushConstants is not an expressive name. Rename to something like TracerConfig
    double rayIdStart;
    double numRays;
    double randomSeed;
    double maxEvents;
    double sequential;
};

struct _debug_struct {
    dmat4 _dMat; // Can also be used as vectors or scalar
};

// we don't require forward declarations in GLSL, hence we only do them in C++:
#ifndef GLSL
extern int gl_GlobalInvocationID;
extern bool inv_finalized;
extern uint64_t inv_ctr;
extern ShaderArray<Ray> RAYX_API inv_rayData;
extern ShaderArray<Ray> RAYX_API inv_outputData;
extern ShaderArray<Element> RAYX_API inv_elements;
extern ShaderArray<dvec4> RAYX_API inv_xyznull;
extern ShaderArray<int> RAYX_API inv_matIdx;
extern ShaderArray<double> RAYX_API inv_mat;

#ifdef RAYX_DEBUG_MODE
extern ShaderArray<_debug_struct> inv_d_struct;
#endif

extern PushConstants inv_pushConstants;
#endif

// Every shader execution calculates the route for a single ray.
// `_ray` is that ray, it's always in world coordinates (!).
#define _ray (inv_rayData[uint(gl_GlobalInvocationID)])

#endif
