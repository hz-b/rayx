// The InvocationState stores all shader-global declarations, including the buffers and stuff like the random-state.
// All InvocationState variables (except for inv.globalInvocationId) are prefixed with `inv_` to separate them from other identifiers.

// TODO If we want to have a multi-threaded CpuTracer, we will require to have multiple InvocationStates at once.
// Thus we probably want to collect these variables into a `struct InvocationState`, from which each thread has a copy.
// - The ShaderArrays from these different copies should probably reference the same buffers though!

// This struct idea might not work well with GLSLs `layout` interpretation of ShaderArrays though.
// In other words, `inv.elements` probably needs to be a global variable, as `GLSL` has to declare it as a global variable using `layout`.
// This problem requires further thought...

#pragma once

#include <span>

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

    // these variables are only used during shader invocation
    int globalInvocationId;
    bool finalized;
    uint64_t ctr;
    uint64_t nextEventIndex;

    std::span<const Ray> rayData;
    std::span<Ray> outputData;
    std::span<int> outputRayCounts;
    std::span<const Element> elements;
    std::span<const dvec4> xyznull;
    std::span<const int> matIdx;
    std::span<const double> mat;

#ifdef RAYX_DEBUG_MODE
    std::span<_debug_struct> d_struct;
#endif

    // TODO(Sven): make all inputs const
    PushConstants pushConstants;
};

using Inv = InvocationState;
