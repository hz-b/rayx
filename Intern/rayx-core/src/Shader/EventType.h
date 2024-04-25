#pragma once

#include "Common.h"

namespace RAYX {

// The meaning of the `m_eventType` field of a `Ray`:
////////////////////////////////////////////////////

// This Ray has just hit `m_lastElement`.
// And will continue tracing afterwards.
// Ray is in element coordinates of the hit element.
constexpr double ETYPE_JUST_HIT_ELEM = 1;

// If the storage space for the events is insufficient for the amount of events that were recorded in a shader call.
constexpr double ETYPE_TOO_MANY_EVENTS = 2;

// This Ray was absorbed by `m_lastElement`.
// Ray is in element coordinates, relative to `m_lastElement`.
constexpr double ETYPE_ABSORBED = 3;

// This is a yet uninitialized ray from outputData.
// This is the initial weight within outputData, and if less events than `maxEvents` are taken,
// the remaining weights in outputData will stay ETYPE_UNINIT even when returned to the CPU.
constexpr double ETYPE_UNINIT = 4;

// This is an error code.
// Functions like refrac2D can error due to "ray beyond horizon", see Utils.h.
// In that case this is returned as final event.
constexpr double ETYPE_BEYOND_HORIZON = 5;

// This is a general error code that means some assertion failed in the shader.
// This error code is typically generated using `_throw`.
constexpr double ETYPE_FATAL_ERROR = 6;

// These rays have just been emitted and not had any other events
// If there are no other elements the ray has this eventtype
constexpr double ETYPE_EMITTED = 7;

} // namespace RAYX
