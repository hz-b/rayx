// The InvocationState stores all shader-global declarations, including the buffers and stuff like the random-state.

#pragma once

#include <span>

#include "Common.h"
#include "Element.h"
#include "Ray.h"

namespace RAYX {

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

// TODO(Sven): restore RAYX_API attributes for members
struct RAYX_API InvState {
    // these variables are only used during shader invocation
    int globalInvocationId;
    bool finalized;
    uint64_t ctr;
    uint64_t nextEventIndex;

    std::span<const Ray> inputRays;
    std::span<Ray> outputRays;
    std::span<int> outputRayCounts;
    std::span<const Element> elements;
    std::span<const int> matIdx;
    std::span<const double> mat;

#ifdef RAYX_DEBUG_MODE
    std::span<_debug_struct> d_struct;
#endif

    // TODO(Sven): make all inputs const
    PushConstants pushConstants;
};

}  // namespace RAYX
