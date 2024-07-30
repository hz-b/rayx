#pragma once

#include <glm.hpp>

#include "Beamline/Cutout.h"

namespace RAYX {

// checks whether the point (x, z) is within the cutout.
RAYX_FN_ACC bool RAYX_API inCutout(Cutout cutout, double x, double z);

// returns a matrix M where (M[i].x, M[i].z) are the key points of our cutout.
// The key points are typically points on the boundary of the cutout.
RAYX_FN_ACC glm::dmat4 RAYX_API keyCutoutPoints(Cutout cutout);

// returns width and length of the bounding box.
RAYX_FN_ACC glm::dvec2 RAYX_API cutoutBoundingBox(Cutout cutout);

// checks whether c1 is a subset of c2, and prints an error otherwise.
// might not find all subset-violations, but should find most of them.
// (might not find all Ellipsoid vs Trapezoid violations)
RAYX_FN_ACC void RAYX_API assertCutoutSubset(Cutout c1, Cutout c2);

}  // namespace RAYX
