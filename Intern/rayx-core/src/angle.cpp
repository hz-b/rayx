#include "angle.h"

#include <sstream>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Shader/Constants.h"

namespace RAYX {

/// Converts an angle in degrees to radians.
Rad Deg::toRad() const { return Rad(deg * PI / 180); }

/// Converts an angle in radians to degrees.
Deg Rad::toDeg() const { return Deg(rad * 180 / PI); }

double Rad::sin() const { return std::sin(rad); }
double Rad::cos() const { return std::cos(rad); }
double Rad::tan() const { return std::tan(rad); }

} // namespace RAYX
