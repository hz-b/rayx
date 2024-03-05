#pragma once

#include "Beamline/OpticalElement.h"

namespace RAYX{

enum class CylinderDirection { LongRadiusR, ShortRadiusRho };

Surface makeSurface(const DesignElement& dele);

// creates a toroid from the parameters given in ` dele`.
Surface makeToroid(const DesignElement& dele);

// creates a quadric from the parameters given in ` dele`.
Surface makeQuadric(const DesignElement&  dele);

// creates a cubic from the parameters given in ` dele`.
Surface makeCubic(const DesignElement&  dele);

// creates a cubic from the parameters given in ` dele`.
Surface makeEllipsoid(const DesignElement&  dele);

// creates a cubic from the parameters given in ` dele`.
Surface makeCone(const DesignElement&  dele);

// creates a cubic from the parameters given in ` dele`.
Surface makeCylinder(const DesignElement&  dele);

// creates a sphere quadric from the parameters given in ` dele`.
Surface makeDesignSphere(const DesignElement&  dele);

Surface makeSphere(double radius);

// creates a plane surface.
Surface makePlane();

Surface makeParaboloid(const DesignElement& dele);
}