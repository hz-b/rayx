#ifndef COLLISION_H
#define COLLISION_H

#include "Common.h"
#include "InvocationState.h"
#include "Utils.h"
#include "ApplySlopeError.h"

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

/**************************************************************
 *                    Collision Finder
 **************************************************************/

// checks whether `r` collides with the element of the given `id`,
// and returns a Collision accordingly.
template <typename MemSpace>
RAYX_FUNC_INLINE
Collision findCollisionWith(Ray r, uint id, Inv<MemSpace>& inv) {
    // misalignment
    r = rayMatrixMult(r, inv.elements[id].m_inTrans);  // image plane is the x-y plane of the coordinate system
    Collision col = findCollisionInElementCoords(r, inv.elements[id].m_surface, inv.elements[id].m_cutout, false);
    if (col.found) {
        col.elementIndex = int(id);
    }

    SlopeError sE = inv.elements[id].m_slopeError;
    col.normal = applySlopeError(col.normal, sE, 0, inv);

    return col;
}

// Returns the next collision for the ray `_ray`.
template <typename MemSpace>
RAYX_FUNC_INLINE
Collision findCollision(Inv<MemSpace>& inv) {
    // If sequential tracing is enabled, we only check collision with the "next element".
    if (inv.pushConstants.sequential == 1.0) {
        if (_ray.m_lastElement >= inv.elements.size() - 1) {
            Collision col;
            col.found = false;
            return col;
        }
        return findCollisionWith(_ray, uint(_ray.m_lastElement + 1), inv);
    }

    // global coordinates of first intersection point of ray among all elements in beamline
    Collision best_col;
    best_col.found = false;

    // the distance the ray has to travel to reach `best_col`.
    double best_dist = infinity();

    // move ray slightly forward.
    // -> prevents hitting an element very close to the previous collision.
    // -> prevents self-intersection.
    Ray r = _ray;
    r.m_position += r.m_direction * COLLISION_EPSILON;

    // Find intersection points through all elements
    for (uint elementIndex = 0; elementIndex < uint(inv.elements.size()); elementIndex++) {
        Collision current_col = findCollisionWith(r, elementIndex, inv);
        if (!current_col.found) {
            continue;
        }

        dvec3 global_hitpoint = dvec3(inv.elements[elementIndex].m_outTrans * dvec4(current_col.hitpoint, 1));
        double current_dist = length(global_hitpoint - _ray.m_position);

        if (current_dist < best_dist) {
            best_col = current_col;
            best_dist = current_dist;
        }
    }

    return best_col;
}

#endif
