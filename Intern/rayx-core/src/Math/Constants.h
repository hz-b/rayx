#pragma once

#include <numbers>
#include "Core.h"

namespace rayx::math::constants {

/// inverse nanometer-electron volt relationship / reziprocal factor
constexpr double inv_nm_to_evolt = 1239.841984332002622;

constexpr double pi = 3.14159265358979323846264338327950;
// 141592653589793238462643383279502884197169399

// DipoleSource Constants

/// eV_J ElectronVolt to Joule.
constexpr double electron_volt = 1.602176634e-19;

/// c_0 [m/s]. speed of light in vacuum.
constexpr double speed_of_light = 299792458;  // checked 2019-7-25, pb, nist (exact): speed of light in vacuo [m/s]

/// e_0 [As]. elementary charge.
constexpr double elementary_charge = 1.602176634e-19;  // checked 2019-7-25, pb, nist (exact): minus charge of electron [as] or [c]

constexpr double planck = 6.62607015e-34;  // checked 2019-7-25, pb, nist (exact): planck constant [js]

/// \f$\hbar\f$ [Js]. Planck's bar constant.
constexpr double planck_bar = planck / (2 * pi);

/// m_e [kg]. electron mass.
constexpr double electron_mass = 9.1093837015e-31;  // checked 2019-7-25, pb, nist: mass of electron [kg];

/// RAY-UI shortcut for getFactorElectronEnergy
constexpr double factor_electron_energy_sc = 1957;

constexpr double fine_structure_constant = 7.2973525693e-3;  // checked 2020-2-18, pb, nist: fine-structure constant, no units

/// \f$\epsilon_0\f$ [As/Vm]. vacuum electric permittivity.
constexpr double electric_permittivity = 8.8541878128e-12;  // checked 2019-7-25, pb, nist: dielectricity of vacuum, or vacuum electric permittivity
                                                            // [As/Vm] or [F/m]; Standard uncertainty: 0.0000000013e-12

/// \f$G\f$ [?]. RAY's factor appearing in Schwinger equation context
constexpr double factor_schwinger_ray = 1.2556937e15;

/// electric permitivity multiplied by speed of light
constexpr double electric_permitivity_multiplies_speed_of_light = 2.6544187279929558624e-3;

constexpr double electron_radius = 2.8179403205e-6;  // classical electron radius (nm)
// constexpr double ELECTRON_RADIUS = (ELEMENTARY_CHARGE / (SPEED_OF_LIGHT * SPEED_OF_LIGHT)) * 1.e9;  // Classical electron radius in nm

}  // namespace rayx
