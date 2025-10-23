#pragma once

#include <glm.hpp>

#include "Core.h"
#include "Element/Cutout.h"
#include "InvocationState.h"
#include "Rand.h"
#include "Ray.h"

namespace RAYX {

struct RAYX_API CollisionPoint {
    glm::dvec3 hitpoint;
    glm::dvec3 normal;
};
static_assert(std::is_trivially_copyable_v<CollisionPoint>);
using OptCollisionPoint = std::optional<CollisionPoint>;

struct CollisionWithElement {
    CollisionPoint point;
    int elementIndex;
};
static_assert(std::is_trivially_copyable_v<CollisionWithElement>);
using OptCollisionWithElement = std::optional<CollisionWithElement>;

RAYX_FN_ACC OptCollisionPoint getQuadricCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection,
                                                  const Surface::Quadric& __restrict quadric);

RAYX_FN_ACC OptCollisionPoint getCubicCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection,
                                                const Surface::Cubic& __restrict cu);

RAYX_FN_ACC OptCollisionPoint getToroidCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection,
                                                 const Surface::Toroid& __restrict toroid, bool isTriangul);

RAYX_FN_ACC OptCollisionPoint RAYX_API findCollisionInElementCoordsWithoutSlopeError(const glm::dvec3& __restrict rayPosition,
                                                                                     const glm::dvec3& __restrict rayDirection,
                                                                                     const Surface& __restrict surface,
                                                                                     const Cutout& __restrict cutout, bool isTriangul);

RAYX_FN_ACC OptCollisionPoint findCollisionInElementCoords(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection,
                                                           const OpticalElement& __restrict element, Rand& __restrict rand);

RAYX_FN_ACC OptCollisionWithElement findCollisionWithElements(glm::dvec3 rayPosition, glm::dvec3 rayDirection,
                                                              const OpticalElement* __restrict elements, const ObjectTransform* __restrict,
                                                              const int numSources, const int numElements, Rand& __restrict rand);

}  // namespace RAYX
