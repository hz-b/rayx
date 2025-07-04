#pragma once

#include <glm.hpp>

#include "Core.h"
#include "Element/Cutout.h"
#include "InvocationState.h"
#include "Rand.h"
#include "Ray.h"

namespace RAYX {

// A "ray-element" collision.
struct RAYX_API Collision {
    // The point where ray hits the element, in element-coordinates.
    glm::dvec3 hitpoint;

    // The normal of the surface at the hitpoint in element-coordinates.
    glm::dvec3 normal;

    // The index of the element with which ray collides.
    int elementIndex;

    // whether a collision has been found.
    bool found;
};

static_assert(std::is_trivially_copyable_v<Collision>);

RAYX_FN_ACC Collision getQuadricCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection, QuadricSurface q, bool isLens);

RAYX_FN_ACC Collision getToroidCollision(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection, ToroidSurface toroid,
                                         bool isTriangul);

RAYX_FN_ACC Collision RAYX_API findCollisionInElementCoords(const glm::dvec3& __restrict rayPosition, const glm::dvec3& __restrict rayDirection,
                                                            Surface surface, Cutout cutout, bool isTriangul, bool lens = false);

RAYX_FN_ACC Collision findCollisionWith(glm::dvec3 rayPosition, glm::dvec3 rayDirection, const int elementIndex,
                                        const OpticalElement& __restrict element, Rand& __restrict rand);

RAYX_FN_ACC Collision findCollisionSequential(const int elementIndex, glm::dvec3& __restrict rayPosition, glm::dvec3& __restrict rayDirection,
                                              const OpticalElement* __restrict elements, Rand& __restrict rand);

RAYX_FN_ACC Collision findCollisionNonSequential(glm::dvec3 rayPosition, const glm::dvec3& __restrict rayDirection,
                                                 const OpticalElement* __restrict elements, const int numElements, Rand& __restrict rand);

RAYX_FN_ACC Collision findCollision(const int eventIndex, const Sequential sequential, const glm::dvec3& __restrict rayPosition,
                                    const glm::dvec3& __restrict rayDirection, const OpticalElement* __restrict elements, const int numElements,
                                    Rand& __restrict rand);

}  // namespace RAYX
