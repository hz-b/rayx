#pragma once

#include <vector>

#include "Beamline/Objects/MatrixSource.h"
#include "Beamline/OpticalElement.h"
#include "RenderObject.h"

RenderObject traceTriangulation(const RAYX::OpticalElement& element, Device& device);

// ------ Helper functions ------
std::pair<double, double> getRectangularDimensions(const Cutout& cutout);
RAYX::MatrixSource setupMatrixSource(const Cutout& cutout);