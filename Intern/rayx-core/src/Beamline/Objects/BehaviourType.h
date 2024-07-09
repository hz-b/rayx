#pragma once

#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class RZPType { Elliptical, Meriodional };
enum class CentralBeamstop { None, Rectangle, Elliptical };

Behaviour makeBehaviour(const DesignElement& dele);

// creates a Grating Behaviour from the parameters given in `dele`.
Behaviour makeGrating(const DesignElement& dele);

Behaviour makeSlit(const DesignElement& dele);

Behaviour makeRZPBehaviour(const DesignElement& dele);

}  // namespace RAYX