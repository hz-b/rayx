#pragma once
/// Header to include to get all objects defined in RayCore. Don't overuse it or
/// build times will be unnecessariy long.
namespace RAYX{
    class LightSource;
    struct DesignSource;
}

// Light Sources
//#include "Beamline/Objects/CircleSource.h"
#include "Beamline/Objects/DipoleSource.h"
#include "Beamline/Objects/MatrixSource.h"
//#include "Beamline/Objects/PixelSource.h"
#include "Beamline/Objects/PointSource.h"
//#include "Beamline/Objects/SimpleUndulatorSource.h"

// Optical Elements
#include "Beamline/Objects/Cone.h"
#include "Beamline/Objects/Cylinder.h"
#include "Beamline/Objects/Ellipsoid.h"
#include "Beamline/Objects/ImagePlane.h"
#include "Beamline/Objects/Paraboloid.h"
#include "Beamline/Objects/PlaneGrating.h"
#include "Beamline/Objects/PlaneMirror.h"
#include "Beamline/Objects/ReflectionZonePlate.h"
#include "Beamline/Objects/Slit.h"
#include "Beamline/Objects/SphereGrating.h"
#include "Beamline/Objects/SphereMirror.h"
#include "Beamline/Objects/ToroidMirror.h"
