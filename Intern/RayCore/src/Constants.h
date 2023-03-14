#ifndef CONSTANTS
#define CONSTANTS

// useful constants

// inverse nanometer-electron volt relationship / reziprocal factor
const double inm2eV = 1.239852e3;

const double PI = 3.14159265358979323846264338327950;
// 141592653589793238462643383279502884197169399

// -----------------------------------------
//
//    WEIGHT STATUS
//
// -----------------------------------------

// The meaning of the `m_weight` field of a `Ray`
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
// Ray is in element coordinates, relative to `m_lastElement`.
const double W_ABSORBED = 3;

// This is a yet uninitialized ray from outputData.
// This is the initial weight within outputData, and if less snapshots than `maxSnapshots` are taken,
// the remaining weights in outputData will stay W_UNINIT even when returned to the CPU.
const double W_UNINIT = 4;

// This is an error code.
// Functions like refrac2D can error due to "ray beyond horizon", see utils.comp.
// In that case this is returned as final snapshot.
const double W_BEYOND_HORIZON = 5;

// TODO extract this somewhere else
#ifndef GLSL

#include <glm.hpp>
using dmat4 = glm::dmat4;

#endif

struct Element {
    // for alignment reasons, the dmat4s are at the beginning of the struct.
    dmat4 m_inTrans;
    dmat4 m_outTrans;

    // the type of this element, see the TY constants.
    // the type describes how the elementParams need to be interpreted.
    double m_type;
    double m_elementParams[16];

    // the surface type of this element, see the STY constants.
    // the surface type describes how the surfaceParams need to be inerpreted.
    double m_surfaceType;
    double m_surfaceParams[16];

    // general object information
    double m_widthA;
    double m_widthB;
    double m_height;
    double m_slopeError[7];
    double m_azimuthalAngle;
    double m_material;

    // This field is unused, it's only there to guarantee that sizeof(Element) is divisible by sizeof(dmat4).
    // Should guarantee that std430 in GLSL and c++ have the same memory layout for `Element`.
    double m_padding[2];
};

// -----------------------------------------
//
//    OBJECT TYPES
//
// -----------------------------------------

// element types
const int TY_MIRROR = 0;
const int TY_GRATING = 1;
const int TY_SLIT = 2;
const int TY_RZP = 3;
const int TY_IMAGE_PLANE = 4;

// surface types:
const int STY_QUADRIC = 0;
const int STY_TOROID = 1;
const int STY_INF_PLANE = 2;  // an infinite X-Y plane.

#endif
