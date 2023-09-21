#ifndef COLLISION_H
#define COLLISION_H

#include "adapt.h"
// A "ray-element" collision.

/**************************************************************
 *                    Cllision Struct
 **************************************************************/
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

#endif
