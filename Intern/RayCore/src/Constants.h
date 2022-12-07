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

// This Ray is still pathing through objects.
// When the computation is finished weight shall not be `W_IN_COMPUTATION`.
//
// Ray is in world coordinates.
const double W_IN_COMPUTATION = 1;

// This Ray has hit an ImagePlane, this ImagePlane is `m_lastElement`.
//
// Ray is in element coordinates, relative to `m_lastElement`.
const double W_HIT_IMAGE_PLANE = 2;

// This Ray was absorbed by `m_lastElement`.
//
// Ray is in element coordinates, relative to `m_lastElement`.
// TODO not yet used!
const double W_ABSORBED = 3;
/* } */

#endif
