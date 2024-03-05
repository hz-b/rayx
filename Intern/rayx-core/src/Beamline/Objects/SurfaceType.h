#pragma once

#include "Beamline/OpticalElement.h"

namespace RAYX{
Surface makeSurface(const DesignElement& dele);

// creates a toroid from the parameters given in `dobj`.
Surface makeToroid(const DesignElement& dele);

// creates a quadric from the parameters given in `dobj`.
Surface makeQuadric(const DesignElement& dobj);

// creates a cubic from the parameters given in `dobj`.
Surface makeCubic(const DesignElement& dobj);

// creates a cubic from the parameters given in `dobj`.
Surface makeEllipsoid(const DesignElement& dobj);

// creates a cubic from the parameters given in `dobj`.
Surface makeCone(const DesignElement& dobj);

// creates a sphere quadric from the parameters given in `dobj`.
Surface makeSphere(double radius);

// creates a plane surface.
Surface makePlane();

Surface makeParaboloid(const DesignElement& dele);
}