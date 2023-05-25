#ifndef CONSTANTS
#define CONSTANTS

// TODO this is a large include. It's probably not required everywhere
#include "Element.h"

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

// DipoleSource Constants

/// eV_J ElectronVolt to Joule.
const double c_electronVolt = 1.602176634e-19;

/// c_0 [m/s]. speed of light in vacuum.
const double c_speedOfLight = 299792458;  // Checked 2019-7-25, PB, NIST (exact): speed of light in vacuo [m/s]

/// e_0 [As]. elementary charge.
const double c_elementaryCharge = 1.602176634e-19;  // Checked 2019-7-25, PB, NIST (exact): minus charge of electron [As] or [C]

const double Planck = 6.62607015e-34;  // Checked 2019-7-25, PB, NIST (exact): Planck constant [Js]

/// \f$\hbar\f$ [Js]. Planck's bar constant.
const double Planck_bar = Planck / (2 * PI);

/// m_e [kg]. electron mass.
const double c_electronMass = 9.1093837015e-31;  // Checked 2019-7-25, PB, NIST: mass of electron [kg];

const double c_alpha = 7.2973525693e-3;

/// \f$\epsilon_0\f$ [As/Vm]. vacuum electric permittivity.
const double c_electricPermittivity = 8.8541878128e-12;  // Checked 2019-7-25, PB, NIST: dielectricity of vacuum, or vacuum electric permittivity
                                                         // [As/Vm] or [F/m]; Standard uncertainty: 0.0000000013e-12

/// \f$G\f$ [?]. RAY's factor appearing in Schwinger equation context
const double factorSchwinger_RAY = 1.2556937e15;

// std::array<double, 2>test;// = {1, 2};

#endif
