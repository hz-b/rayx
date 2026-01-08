#pragma once

#include "Collision.h"
#include "Core.h"
#include "InvocationState.h"
#include "Ray.h"

namespace rayx {

/// Each `behave*` function gets a `Ray ray` (in element-coords), the element itself, a randomness counter and the `Collision col`.
/// `col.hitpoint` expresses where the given ray will hit the element (in element-coords).
/// `col.normal` expresses the normal of the surface, at `col.hitpoint` (in element-coords).
/// The ray has already been moved to the hitpoint, and it's lastElement has been set accordingly.
///
/// The `behave*` functions, will
/// - change the rays direction, typically by reflecting using the normal
/// - change the rays stokes vector
/// - potentially absorb the ray (by calling `recordFinalEvent(_, EventType::Absorbed)`)

RAYX_FN_ACC void behaveCrystal(detail::Ray& __restrict ray, const Behaviour::Crystal& __restrict crystal, const CollisionPoint& __restrict col);
RAYX_FN_ACC void behaveSlit(detail::Ray& __restrict ray, const Behaviour::Slit& __restrict slit);
RAYX_FN_ACC void behaveRZP(detail::Ray& __restrict ray, const Behaviour::RZP& __restrict rzp, const CollisionPoint& __restrict col);
RAYX_FN_ACC void behaveGrating(detail::Ray& __restrict ray, const Behaviour::Grating& __restrict grating, const CollisionPoint& __restrict col);
RAYX_FN_ACC void behaveMirror(detail::Ray& __restrict ray, const CollisionPoint& __restrict col, const Coating& __restrict coating, int material,
                              const int* __restrict materialIndices, const double* __restrict materialTable);
RAYX_FN_ACC void behaveFoil(detail::Ray& __restrict ray, const Behaviour::Foil& __restrict foil, const CollisionPoint& __restrict col, int material,
                            const int* __restrict materialIndices, const double* __restrict materialTable);
RAYX_FN_ACC void behaveImagePlane(detail::Ray& __restrict ray);
RAYX_FN_ACC void behave(detail::Ray& __restrict ray, const CollisionPoint& __restrict col, const OpticalElement& __restrict element,
                        const int* __restrict materialIndices, const double* __restrict materialTable);

}  // namespace rayx
