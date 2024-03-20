#ifndef COLLISION_H
#define COLLISION_H

#include "Common.h"
#include "InvocationState.h"

#define COLLISION_EPSILON 1e-6

// A "ray-element" collision.
struct RAYX_API Collision {
    // The point where `_ray` hits the element, in element-coordinates.
    dvec3 hitpoint;

    // The normal of the surface at the hitpoint in element-coordinates.
    dvec3 normal;

    // The index of the element with which `_ray` collides.
    int elementIndex;

    // whether a collision has been found.
    bool found;
};

RAYX_FUNC Collision getQuadricCollision(Ray r, QuadricSurface q);
RAYX_FUNC Collision getToroidCollision(Ray r, ToroidSurface toroid, bool isTriangul);
RAYX_FUNC Collision RAYX_API findCollisionInElementCoords(Ray r, Surface surface, Cutout cutout, bool isTriangul);
RAYX_FUNC Collision findCollisionWith(Ray r, uint id, Inv& inv);
RAYX_FUNC Collision findCollision(Inv& inv);

#endif
