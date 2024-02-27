// The InvocationState stores all shader-global declarations, including the buffers and stuff like the random-state.
// All InvocationState variables (except for inv.globalInvocationId) are prefixed with `inv_` to separate them from other identifiers.

// TODO If we want to have a multi-threaded CpuTracer, we will require to have multiple InvocationStates at once.
// Thus we probably want to collect these variables into a `struct InvocationState`, from which each thread has a copy.
// - The ShaderArrays from these different copies should probably reference the same buffers though!

// This struct idea might not work well with GLSLs `layout` interpretation of ShaderArrays though.
// In other words, `inv.elements` probably needs to be a global variable, as `GLSL` has to declare it as a global variable using `layout`.
// This problem requires further thought...

#ifndef INVOCATION_STATE_H
#define INVOCATION_STATE_H

#include "Common.h"
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

// TODO(Sven): restore RAYX_API attributes for members
struct RAYX_API InvocationState {
    int globalInvocationId;
    bool finalized;
    uint64_t ctr;
    uint64_t nextEventIndex;
    ShaderArray<Ray> rayData;
    ShaderArray<Ray> outputData;
    ShaderArray<Element> elements;
    ShaderArray<dvec4> xyznull;
    ShaderArray<int> matIdx;
    ShaderArray<double> mat;

#ifdef RAYX_DEBUG_MODE
    ShaderArray<_debug_struct> d_struct;
#endif

    PushConstants pushConstants;
};

using Inv = InvocationState;

// Every shader execution calculates the route for a single ray.
// `_ray` is that ray, it's always in world coordinates (!).
#define _ray (inv.rayData[uint(inv.globalInvocationId)])

#endif
