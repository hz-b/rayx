#pragma once

#include "Collision.h"
#include "Core.h"
#include "InvocationState.h"
#include "Ray.h"

namespace RAYX {

/// Each `behave*` function gets a `Ray ray` (in element-coords), the element itself, a randomness counter and the `Collision col`.
/// `col.hitpoint` expresses where the given ray will hit the element (in element-coords).
/// `col.normal` expresses the normal of the surface, at `col.hitpoint` (in element-coords).
/// The ray has already been moved to the hitpoint, and it's lastElement has been set accordingly.
///
/// The `behave*` functions, will
/// - change the rays direction, typically by reflecting using the normal
/// - change the rays stokes vector
/// - potentially absorb the ray (by calling `recordFinalEvent(_, EventType::Absorbed)`)

RAYX_FN_ACC void behaveCrystal(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col);
RAYX_FN_ACC void behaveSlit(Ray& __restrict ray, const Behaviour& __restrict behaviour);
RAYX_FN_ACC void behaveRZP(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col);
RAYX_FN_ACC void behaveGrating(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col);
RAYX_FN_ACC void behaveMirror(Ray& __restrict rar, const CollisionPoint& __restrict col, int material, const Material materials);
RAYX_FN_ACC void behaveFoil(Ray& __restrict ray, const Behaviour& __restrict behaviour, const CollisionPoint& __restrict col, int material,
                            const Materials materials);
RAYX_FN_ACC void behaveImagePlane(Ray& __restrict ray);
RAYX_FN_ACC void behave(Ray& __restrict ray, const CollisionPoint& __restrict col, const OpticalElement& __restrict element,
                        const Materials materials);

}  // namespace RAYX
