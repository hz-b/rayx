#pragma once

#include "Beamline/OpticalElement.h"

namespace RAYX {

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

Surface makeSurface(const DesignElement& dele);

// creates a toroid from the parameters given in ` dele`.
Surface makeToroid(const DesignElement& dele);

// creates a quadric from the parameters given in ` dele`.
Surface makeQuadric(const DesignElement& dele);

// creates a cubic from the parameters given in ` dele`.
Surface makeCubic(const DesignElement& dele);

// creates a Ellipsoid from the parameters given in ` dele`.
Surface makeEllipsoid(const DesignElement& dele);

// creates a Cone from the parameters given in ` dele`.
Surface makeCone(const DesignElement& dele);

// creates a Cylinder from the parameters given in ` dele`.
Surface makeCylinder(const DesignElement& dele);

// creates a sphere from the radius .
Surface makeSphere(double radius);

// creates a plane surface.
Surface makePlane();

// creates a Paraboloid from the parameters given in ` dele`.
Surface makeParaboloid(const DesignElement& dele);
}  // namespace RAYX