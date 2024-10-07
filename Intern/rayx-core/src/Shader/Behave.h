#pragma once

#include "Collision.h"
#include "Core.h"
#include "InvocationState.h"
#include "Ray.h"

namespace RAYX {

/// Each `behave*` function gets a `Ray r` (in element-coords), the element itself, a randomness counter and the `Collision col`.
/// `col.hitpoint` expresses where the given ray r will hit the element (in element-coords).
/// `col.normal` expresses the normal of the surface, at `col.hitpoint` (in element-coords).
/// The ray has already been moved to the hitpoint, and it's lastElement has been set accordingly.
///
/// The `behave*` functions, will
/// - change the rays direction, typically by reflecting using the normal
/// - change the rays stokes vector
/// - potentially absorb the ray (by calling `recordFinalEvent(_, EventType::Absorbed)`)

RAYX_FN_ACC Ray behaveSlit(Ray r, int id, Collision col, InvState& inv);
RAYX_FN_ACC Ray behaveRZP(Ray r, int id, Collision col, InvState& inv);
RAYX_FN_ACC Ray behaveGrating(Ray r, int id, Collision col, InvState& inv);
RAYX_FN_ACC Ray behaveMirror(Ray r, int id, Collision col, InvState& inv);
RAYX_FN_ACC Ray behaveImagePlane(Ray r, int id, Collision col, InvState& inv);

}  // namespace RAYX
