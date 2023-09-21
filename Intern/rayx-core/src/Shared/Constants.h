#ifndef CONSTANTS
#define CONSTANTS

#include "EventType.h"
#include "ImageType.h"

// useful constants

// inverse nanometer-electron volt relationship / reziprocal factor
const float inm2eV = 1.239852e3;

const float PI = 3.14159265358979323846264338327950;
// 141592653589793238462643383279502884197169399

// DipoleSource Constants

/// eV_J ElectronVolt to Joule.
const float c_electronVolt = 1.602176634e-19;

/// c_0 [m/s]. speed of light in vacuum.
const float c_speedOfLight = 299792458;  // Checked 2019-7-25, PB, NIST (exact): speed of light in vacuo [m/s]

/// e_0 [As]. elementary charge.
const float c_elementaryCharge = 1.602176634e-19;  // Checked 2019-7-25, PB, NIST (exact): minus charge of electron [As] or [C]

const float c_Planck = 6.62607015e-34;  // Checked 2019-7-25, PB, NIST (exact): Planck constant [Js]

/// \f$\hbar\f$ [Js]. Planck's bar constant.
const float c_Planck_bar = c_Planck / (2 * PI);

/// m_e [kg]. electron mass.
const float c_electronMass = 9.1093837015e-31;  // Checked 2019-7-25, PB, NIST: mass of electron [kg];

//
const float c_fineStructureConstant = 7.2973525693e-3; // Checked 2020-2-18, PB, NIST: fine-structure constant, no units

/// \f$\epsilon_0\f$ [As/Vm]. vacuum electric permittivity.
const float c_electricPermittivity = 8.8541878128e-12;  // Checked 2019-7-25, PB, NIST: dielectricity of vacuum, or vacuum electric permittivity
                                                         // [As/Vm] or [F/m]; Standard uncertainty: 0.0000000013e-12

/// \f$G\f$ [?]. RAY's factor appearing in Schwinger equation context
const float c_factorSchwinger_RAY = 1.2556937e15;

// std::array<float, 2>test;// = {1, 2};



#define PlaneDir float
// the direction of a plane.
// Either XY or XZ.

const PlaneDir PLANE_XY = 0;
const PlaneDir PLANE_XZ = 1;

#endif
