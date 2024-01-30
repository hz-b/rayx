// The InvocationState stores all shader-global declarations, including the buffers and stuff like the random-state.
// All InvocationState variables (except for gl_GlobalInvocationID) are prefixed with `inv_` to separate them from other identifiers.

// TODO If we want to have a multi-threaded CpuTracer, we will require to have multiple InvocationStates at once.
// Thus we probably want to collect these variables into a `struct InvocationState`, from which each thread has a copy.
// - The ShaderArrays from these different copies should probably reference the same buffers though!

// This struct idea might not work well with GLSLs `layout` interpretation of ShaderArrays though.
// In other words, `inv_elements` probably needs to be a global variable, as `GLSL` has to declare it as a global variable using `layout`.
// This problem requires further thought...

#ifndef INVOCATION_STATE_H
#define INVOCATION_STATE_H

#include "Adapt.h"
#include "Ray.h"

// Useful for GPU Tracing
struct PushConstants {  // TODO(Jannis): PushConstants is not an expressive name. Rename to something like TracerConfig
    double rayIdStart;
    double numRays;
    double randomSeed;
    double maxEvents;
    double sequential;
    double startEventID;
};

struct _debug_struct {
    dmat4 _dMat;  // Can also be used as vectors or scalar
};

// we don't require forward declarations in GLSL, hence we only do them in C++:
#ifndef GLSL
extern int gl_GlobalInvocationID;
extern bool inv_finalized;
extern uint64_t inv_ctr;
extern uint64_t inv_nextEventIndex;
extern ShaderArray<Ray> RAYX_API inv_rayData;
extern ShaderArray<Ray> RAYX_API inv_outputData;
extern ShaderArray<Element> RAYX_API inv_elements;
extern ShaderArray<dvec4> RAYX_API inv_xyznull;
extern ShaderArray<int> RAYX_API inv_matIdx;
extern ShaderArray<double> RAYX_API inv_mat;

#ifdef RAYX_DEBUG_MODE
extern ShaderArray<_debug_struct> RAYX_API inv_d_struct;
#endif

extern PushConstants inv_pushConstants;
#endif

// Every shader execution calculates the route for a single ray.
// `_ray` is that ray, it's always in world coordinates (!).
#define _ray (inv_rayData[uint(gl_GlobalInvocationID)])

#endif
