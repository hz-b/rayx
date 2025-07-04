#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
void dynamicElements(const int gid, const InvState& inv, OutputEvents& outputEvents) {
    auto ray = inv.rays[gid];
    auto rand = Rand(gid + inv.batchStartRayIndex, inv.numRaysTotal, inv.randomSeed);

    // Iterate through all bounces
    int numRecorded = 0;
    bool colNotFound = false;
    for (int bounce = 0; bounce < inv.maxEvents; ++bounce) {
        // the ray might finalize due to being absorbed, or because an error occured while tracing!
        if (!isRayActive(ray.m_eventType)) break;

        Collision col = findCollision(bounce, inv.sequential, ray.m_position, ray.m_direction, inv.elements, inv.numElements, rand);
        if (!col.found) {
            // no element was hit.
            // Tracing is done!
            colNotFound = true;
            break;
        }

        // transform ray and intersection point in ELEMENT coordiantes
        const auto element = inv.elements[col.elementIndex];
        ray = rayMatrixMult(element.m_inTrans, ray);

        // Calculate interaction(reflection,material, absorption etc.) of ray with detected next element
        const auto behaviour = element.m_behaviour;

        ray.m_pathLength += glm::length(ray.m_position - col.hitpoint);
        ray.m_position = col.hitpoint;
        ray.m_lastElement = col.elementIndex;
        ray.m_eventType = EventType::HitElement;

        switch (behaviour.m_type) {
            case BehaveType::Mirror:
                ray = behaveMirror(ray, col, element.m_material, inv.materialIndices, inv.materialTables);
                break;
            case BehaveType::Grating:
                ray = behaveGrating(ray, behaviour, col);
                break;
            case BehaveType::Slit:
                ray = behaveSlit(ray, behaviour, rand);
                break;
            case BehaveType::RZP:
                ray = behaveRZP(ray, behaviour, col, rand);
                break;
            case BehaveType::Crystal:
                ray = behaveCrystal(ray, behaviour, col);
                break;
            case BehaveType::ImagePlane:
                ray = behaveImagePlane(ray);
                break;
            case BehaveType::Foil:
                ray = behaveFoil(ray, behaviour, col, element.m_material, inv.materialIndices, inv.materialTables);
                break;
            case BehaveType::Lens:
                auto surface = element.m_surface;
                const auto cutout = element.m_cutout;
                ray = behaveLens(ray, behaviour, col, element.m_material, inv.materialIndices, inv.materialTables, surface, cutout);
                break;
        }

        // write ray in local element coordinates to global memory
        if (numRecorded < inv.maxEvents && (!inv.recordMask || inv.recordMask[col.elementIndex])) {
            outputEvents.events[gid * inv.maxEvents + numRecorded] = ray;
            ++numRecorded;
        }

        // transform back to WORLD coordinates
        ray = rayMatrixMult(element.m_outTrans, ray);
    }

    // check if the number of events exceeds capacity
    if (!colNotFound && inv.sequential == Sequential::No && isRayActive(ray.m_eventType)) {
        Collision col = findCollisionNonSequential(ray.m_position, ray.m_direction, inv.elements, inv.numElements, rand);
        if (col.found) {
            ray = terminateRay(ray, EventType::TooManyEvents);
            outputEvents.events[gid * inv.maxEvents + inv.maxEvents - 1] = ray;
        }
    }

    // store recorded events count
    outputEvents.eventCounts[gid] = numRecorded;
}

}  // namespace RAYX
