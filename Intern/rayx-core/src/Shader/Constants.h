// The shader-internal constants are mostly defined in this file.

#ifndef CONSTANTS
#define CONSTANTS

#include "Common.h"
#include "EventType.h"
#include "ImageType.h"

// inverse nanometer-electron volt relationship / reziprocal factor
const double inm2eV = 1.239852e3; // TODO: Check, find better name maybe

const double PI = 3.14159265358979323846264338327950;
// 141592653589793238462643383279502884197169399

// DipoleSource Constants

/// eV_J ElectronVolt to Joule.
const double ELECTRON_VOLT = 1.602176634e-19;

/// c_0 [m/s]. speed of light in vacuum.
const double SPEED_OF_LIGHT = 299792458;  // Checked 2019-7-25, PB, NIST (exact): speed of light in vacuo [m/s]

/// e_0 [As]. elementary charge.
const double ELEMENTARY_CHARGE = 1.602176634e-19;  // Checked 2019-7-25, PB, NIST (exact): minus charge of electron [As] or [C]

const double PLANCK = 6.62607015e-34;  // Checked 2019-7-25, PB, NIST (exact): Planck constant [Js]

/// \f$\hbar\f$ [Js]. Planck's bar constant.
const double PLANCK_BAR = PLANCK / (2 * PI);

/// m_e [kg]. electron mass.
const double ELECTRON_MASS = 9.1093837015e-31;  // Checked 2019-7-25, PB, NIST: mass of electron [kg];

/// RAY-UI shortcut for getFactorElectronEnergy
const double FACTOR_ELECTRON_ENERGY_SC = 1957;

//
const double FINE_STRUCTURE_CONSTANT = 7.2973525693e-3;  // Checked 2020-2-18, PB, NIST: fine-structure constant, no units

/// \f$\epsilon_0\f$ [As/Vm]. vacuum electric permittivity.
const double ELECTRIC_PERMITTIVITY = 8.8541878128e-12;  // Checked 2019-7-25, PB, NIST: dielectricity of vacuum, or vacuum electric permittivity
                                                        // [As/Vm] or [F/m]; Standard uncertainty: 0.0000000013e-12

/// \f$G\f$ [?]. RAY's factor appearing in Schwinger equation context
const double FACTOR_SCHWINGER_RAY = 1.2556937e15;

#endif
