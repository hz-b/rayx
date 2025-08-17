#pragma once

#include "Element/Element.h"
#include "Rand.h"
#include "Ray.h"
#include "RecordMask.h"

namespace RAYX {

/// Expresses whether we force sequential tracing, or we use dynamic tracing.
/// We prefer this over a boolean, as calling eg. the trace function with an argument of `true` has no obvious meaning.
/// On the other hand calling it with `Sequential::Yes` makes the meaning more clear.
enum class Sequential { No, Yes };

/// stores all constant buffers
struct RAYX_API ConstState {
    int maxEvents;
    Sequential sequential = Sequential::No;

    OpticalElement* __restrict elements;
    int numElements;
    int* __restrict materialIndices;
    double* __restrict materialTables;
    RecordMask recordMask;  //< Mask that decides which elements to record events for (array length is numElements)
    Ray* __restrict rays;
};

/// stores all mutable buffers
struct RAYX_API MutableState {
    Ray* __restrict events;
    int* __restrict eventCounts;
    Rand* __restrict rands;
};

}  // namespace RAYX
