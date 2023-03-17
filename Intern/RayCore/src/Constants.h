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

// This is a general error code that means some assertion failed in the shader.
// This error code is typically generated using `throw`.
const double W_FATAL_ERROR = 6;

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
    // the surface type describes how the surfaceParams need to be interpreted.
    double m_surfaceType;
    double m_surfaceParams[16];

    // describes how cutoutParams are interpreted, see the CTY constants.
    double m_cutoutType;
    double m_cutoutParams[3];

    // general object information
    double m_slopeError[7];
    double m_azimuthalAngle;
    double m_material;

    // This field is unused, it's only there to guarantee that sizeof(Element) is divisible by sizeof(dmat4).
    // Should guarantee that std430 in GLSL and c++ have the same memory layout for `Element`.
    double m_padding[1];
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
// a surface is a potentially infinite curved surface in 3d space.
// as our elements are mostly finite in size, they are represented by a (potentially infinite) surface in combination with a finite cutout (see CTY
// constants)
const int STY_QUADRIC = 0;
const int STY_TOROID = 1;
const int STY_PLANE_XY = 2;  // an infinite X-Y plane.

// cutout types:
// a subset of points in the 2d plane. used to limited the potentially infinite surfaces.
// note that the first 3 need to be RECT; ELLIPTICAL; TRAPEZOID in order to be compatible with Ray-UI.
const int CTY_RECT = 0;  // cutout parameters are (width, 0, length).
const int CTY_ELLIPTICAL = 1;
const int CTY_TRAPEZOID = 2;
const int CTY_UNLIMITED = 3;  // cutout parameters are (0, 0, 0).

#endif
