#pragma once

#include <glm.h>

#include "Core.h"
#include "Element/Cutout.h"
#include "InvocationState.h"
#include "Ray.h"

#define COLLISION_EPSILON 1e-6

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

RAYX_FN_ACC Collision getQuadricCollision(Ray r, QuadricSurface q);
RAYX_FN_ACC Collision getToroidCollision(Ray r, ToroidSurface toroid, bool isTriangul);
RAYX_FN_ACC Collision RAYX_API findCollisionInElementCoords(Ray r, Surface surface, Cutout cutout, bool isTriangul);
RAYX_FN_ACC Collision findCollisionWith(Ray r, uint id, InvState& inv);
RAYX_FN_ACC Collision findCollision(const Ray& ray, InvState& inv);

}  // namespace RAYX
