#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

// The meaning of the `m_eventType` field of a `Ray`:
////////////////////////////////////////////////////

// The future Path of this Ray does not intersect any elements anymore.
// This Ray now flies off to infinity.
//
// Ray is in world coordinates.
const float ETYPE_FLY_OFF = 0;

// This Ray has just hit `m_lastElement`.
// And will continue tracing afterwards.
// Ray is in element coordinates of the hit element.
const float ETYPE_JUST_HIT_ELEM = 1;

// The ray has found another collision, but the `maxBounces` cap was reached.
const float ETYPE_NOT_ENOUGH_BOUNCES = 2;

// This Ray was absorbed by `m_lastElement`.
// Ray is in element coordinates, relative to `m_lastElement`.
const float ETYPE_ABSORBED = 3;

// This is a yet uninitialized ray from outputData.
// This is the initial weight within outputData, and if less events than `maxEvents` are taken,
// the remaining weights in outputData will stay ETYPE_UNINIT even when returned to the CPU.
const float ETYPE_UNINIT = 4;

// This is an error code.
// Functions like refrac2D can error due to "ray beyond horizon", see utils.comp.
// In that case this is returned as final event.
const float ETYPE_BEYOND_HORIZON = 5;

// This is a general error code that means some assertion failed in the shader.
// This error code is typically generated using `throw`.
const float ETYPE_FATAL_ERROR = 6;

#endif
