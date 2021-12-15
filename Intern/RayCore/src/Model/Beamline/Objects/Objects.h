#pragma once
/// Header to include to get all objects defined in RayCore. Don't overuse it or
/// build times will be unnecessariy long.

// Light Sources
#include "Model/Beamline/Objects/MatrixSource.h"
#include "Model/Beamline/Objects/PointSource.h"
#include "Model/Beamline/Objects/RandomRays.h"

// Optical Elements
#include "Model/Beamline/Objects/Ellipsoid.h"
#include "Model/Beamline/Objects/ImagePlane.h"
#include "Model/Beamline/Objects/PlaneGrating.h"
#include "Model/Beamline/Objects/PlaneMirror.h"
#include "Model/Beamline/Objects/ReflectionZonePlate.h"
#include "Model/Beamline/Objects/Slit.h"
#include "Model/Beamline/Objects/SphereGrating.h"
#include "Model/Beamline/Objects/SphereMirror.h"
#include "Model/Beamline/Objects/ToroidMirror.h"