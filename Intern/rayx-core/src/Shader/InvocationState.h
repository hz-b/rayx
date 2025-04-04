
#pragma once

#include "Element/Element.h"
#include "Ray.h"

namespace RAYX {

/// Expresses whether we force sequential tracing, or we use dynamic tracing.
/// We prefer this over a boolean, as calling eg. the trace function with an argument of `true` has no obvious meaning.
/// On the other hand calling it with `Sequential::Yes` makes the meaning more clear.
enum class Sequential { No, Yes };

// The InvocationState stores all shader-global declarations, including the buffers and stuff like the random-state.
struct RAYX_API InvState {
    int numRaysTotal;
    int numRaysBatch;
    int batchStartRayIndex;
    int maxEvents;
    double randomSeed;
    Sequential sequential;

    OpticalElement* elements;
    int numElements;
    int* materialIndices;
    double* materialTables;
    Ray* inputRays;
};

struct RAYX_API OutputEvents {
    Ray* events;
    int* numEvents;
};

}  // namespace RAYX
