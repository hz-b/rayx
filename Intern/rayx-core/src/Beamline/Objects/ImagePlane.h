#pragma once
#include "Beamline/OpticalElement.h"
#include "DesignElement/DesignElement.h"

namespace RAYX {
struct DesignElement; //TODO Fanny see where the forward declaration has to go
Element makeImagePlane(const DesignElement& dele);

}  // namespace RAYX
