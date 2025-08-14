
#pragma once

#include "Element/Element.h"
#include "Rand.h"
#include "Ray.h"

namespace RAYX {

/// Expresses whether we force sequential tracing, or we use dynamic tracing.
/// We prefer this over a boolean, as calling eg. the trace function with an argument of `true` has no obvious meaning.
/// On the other hand calling it with `Sequential::Yes` makes the meaning more clear.
enum class Sequential { No, Yes };

/// stores all constant buffers
struct RAYX_API ConstState {
    int maxEvents;
    Sequential sequential = Sequential::No;

    OpticalElement* elements;
    int numElements;
    int* materialIndices;
    double* materialTables;
    bool* recordMask;  //< Mask that decides which elements to record events for (array length is numElements)
    Ray* rays;
};

/// stores all mutable buffers
struct RAYX_API MutableState {
    Ray* events;
    int* eventCounts;
    Rand* rands;
};

}  // namespace RAYX
