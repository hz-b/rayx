#pragma once

#include "Core.h"

namespace RAYX {

enum class EventType {

    // The meaning of the `m_eventType` field of a `Ray`:
    ////////////////////////////////////////////////////

    // This Ray has just hit `m_lastElement`.
    // And will continue tracing afterwards.
    // Ray is in element coordinates of the hit element.
    HitElement = 1,

    // If the storage space for the events is insufficient for the amount of events that were recorded in a shader call.
    TooManyEvents = 2,

    // This Ray was absorbed by `m_lastElement`.
    // Ray is in element coordinates, relative to `m_lastElement`.
    Absorbed = 3,

    // This is a yet uninitialized ray from outputData.
    // This is the initial weight within outputData, and if less events than `maxEvents` are taken,
    // the remaining weights in outputData will stay EventType::Uninitialized even when returned to the CPU.
    Uninitialized = 4,

    // This is an error code.
    // Functions like refrac2D can error due to "ray beyond horizon", see Utils.h.
    // In that case this is returned as final event.
    BeyondHorizon = 5,

    // This is a general error code that means some assertion failed in the shader.
    // This error code is typically generated using `_throw`.
    FatalError = 6,

    // These rays have just been emitted and not had any other events
    // If there are no other elements the ray has this eventtype
    Emitted = 7,

};

}  // namespace RAYX
