#pragma once

#include "Element/Element.h"
#include "ObjectMask.h"
#include "RaysPtr.h"

namespace RAYX {

/// Expresses whether we force sequential tracing, or we use dynamic tracing.
/// We prefer this over a boolean, as calling eg. the trace function with an argument of `true` has no obvious meaning.
/// On the other hand calling it with `Sequential::Yes` makes the meaning more clear.
enum class Sequential { No, Yes };

struct Materials {
    int* __restrict indices;
    double* __restrict tables;
};

/// stores all constant buffers
struct RAYX_API ConstState {
    int maxEvents;
    Sequential sequential = Sequential::No;
    int numSources;
    int numElements;
    int outputEventsGridStride;

    ObjectTransform* __restrict objectTransforms;
    OpticalElement* __restrict elements;
    Materials materials;
    bool* __restrict objectRecordMask;  // Mask that decides which elements to record events for (array length is numElements)
    RayAttrMask attrRecordMask;
    RaysPtr rays;
};

/// stores all mutable buffers
struct RAYX_API MutableState {
    RaysPtr events;
    bool* __restrict storedFlags;
};

}  // namespace RAYX
