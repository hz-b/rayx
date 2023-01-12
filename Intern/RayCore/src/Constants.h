#ifndef CONSTANTS
#define CONSTANTS

// useful constants

// inverse nanometer-electron volt relationship / reziprocal factor
const double inm2eV = 1.239852e3;

const double PI = 3.14159265358979323846264338327950;
// 141592653589793238462643383279502884197169399

/* The meaning of the `m_weight` field of a `Ray` { */
// The future Path of this Ray does not intersect any elements anymore (in other words `getOpticalElementIntersection` yields w=0 for all elements)
// This Ray now flies off to infinity.
//
// Ray is in world coordinates.
const double W_FLY_OFF = 0;

// This Ray has just hit `m_lastElement`.
// And will continue tracing afterwards.
// Ray is in element coordinates of the hit element.
const double W_JUST_HIT_ELEM = 1;

// The ray has found another collision, but the `maxBounces` cap was reached.
const double W_NOT_ENOUGH_BOUNCES = 2;

// This Ray was absorbed by `m_lastElement`.
//
// Ray is in element coordinates, relative to `m_lastElement`.
// TODO not yet used!
const double W_ABSORBED = 3;

// This is a yet uninitialized ray from outputData.
// This is the initial weight within outputData, and if less snapshots than `maxSnapshots` are taken,
// the remaining weights in outputData will stay W_UNINIT even when returned to the CPU.
const double W_UNINIT = 4;
/* } */

// The meaning of surfaceParams[3][0] (i.e. the type of an element):
const double TY_BASIC_MIRROR = 0;  // plane + sphere mirror
const double TY_PLANE_GRATING = 1;
const double TY_GENERAL_GRATING = 2;
const double TY_SLIT = 3;
const double TY_RZP = 4;
const double TY_IMAGE_PLANE = 5;
const double TY_TOROID_MIRROR = 6;
const double TY_ELLIPSOID_MIRROR = 7;

#endif
