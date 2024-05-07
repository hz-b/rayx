#pragma once
/// Header to include to get all objects defined in RayCore. Don't overuse it or
/// build times will be unnecessariy long.
namespace RAYX{
    class LightSource;
    struct DesignSource;
}

// Light Sources
#include "Beamline/Objects/CircleSource.h"
#include "Beamline/Objects/DipoleSource.h"
#include "Beamline/Objects/MatrixSource.h"
#include "Beamline/Objects/PixelSource.h"
#include "Beamline/Objects/PointSource.h"
#include "Beamline/Objects/SimpleUndulatorSource.h"
