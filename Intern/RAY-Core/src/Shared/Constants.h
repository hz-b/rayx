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


//DipoleSource Constants

/// eV_J ElectronVolt to Joule.
const double c_electronVolt = 1.602176634e-19;

/// c_0 [m/s]. speed of light in vacuum.
const double c_speedOfLight = 299792458;            // Checked 2019-7-25, PB, NIST (exact): speed of light in vacuo [m/s]

/// e_0 [As]. elementary charge.
const double c_elementaryCharge = 1.602176634e-19;     // Checked 2019-7-25, PB, NIST (exact): minus charge of electron [As] or [C]

const double Planck = 6.62607015e-34;    // Checked 2019-7-25, PB, NIST (exact): Planck constant [Js]

/// \f$\hbar\f$ [Js]. Planck's bar constant.
const double Planck_bar = Planck/(2*PI);

/// m_e [kg]. electron mass.
const double c_electronMass = 9.1093837015e-31;		// Checked 2019-7-25, PB, NIST: mass of electron [kg];
    
const double alpha = 7.2973525693e-3;

/// \f$\epsilon_0\f$ [As/Vm]. vacuum electric permittivity.
const double c_electricPermittivity = 8.8541878128e-12;// Checked 2019-7-25, PB, NIST: dielectricity of vacuum, or vacuum electric permittivity [As/Vm] or [F/m];
                                                        // Standard uncertainty: 0.0000000013e-12

/// \f$G\f$ [?]. RAY's factor appearing in Schwinger equation context
const double factorSchwinger_RAY = 1.2556937e15;

//std::array<double, 2>test;// = {1, 2};

/*std::array<double, 59>SchwingerX = {1.e-4,1.e-3,2.e-3,4.e-3,6.e-3,8.e-3,1.e-2,2.e-2,\
	3.e-2,4.e-2,5.e-2,6.e-2,7.e-2,8.e-2,9.e-2,1.e-1,0.15,\
	0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,\
	0.7,0.75,0.8,0.85,0.9,1.0,1.25,1.5,1.75,2.0,\
	2.25,2.5,2.75,3.0,3.25,3.5,3.75,4.0,4.25,\
	4.5,4.75,5.0,5.5,6.0,6.5,7.0,7.5,8.0,8.5,\
	9.0,9.5,10.0};


// winick G_0(y) bessel functions
std::array<double, 59>SchwingerY = {973.0,213.6,133.6,83.49,63.29,51.92,44.5,
	27.36,20.45,16.57,14.03,12.22,10.85,9.777,8.905,
	8.182,5.832,4.517,3.663,3.059,2.607,2.255,1.973,
	1.742,1.549,1.386,1.246,1.126,1.02,9.28e-1,8.465e-1,
	7.74e-1,6.514e-1,4.359e-1,3.004e-1,2.113e-1,1.508e-1,1.089e-1,
	7.926e-2,5.811e-2,4.286e-2,3.175e-2,2.362e-2,1.764e-2,1.321e-2,
	9.915e-3,7.461e-3,5.626e-3,4.25e-3,2.436e-3,1.404e-3,8.131e-4,
	4.842e-4,2.755e-4,1.611e-4,9.439e-5,5.543e-5,3.262e-5,1.922e-5};
*/

#endif
