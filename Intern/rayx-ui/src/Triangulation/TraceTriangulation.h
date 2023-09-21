#pragma once

#include <vector>

#include "Beamline/Objects/MatrixSource.h"
#include "Beamline/OpticalElement.h"
#include "RenderObject.h"

std::vector<RenderObject> traceTriangulation(const std::vector<RAYX::OpticalElement>& elements);

// ------ Helper functions ------
std::pair<double, double> getRectangularDimensions(const Cutout& cutout);
RAYX::MatrixSource setupMatrixSource(const Cutout& cutout);