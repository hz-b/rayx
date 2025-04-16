#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "Utils.h"

namespace RAYX {

// TODO: EventType::TooManyEvents is currently not used, but should be used to indicate that the trace job missed information in recorded output

RAYX_FN_ACC
void dynamicElements(const int gid, const InvState& inv, OutputEvents& outputEvents) {
    auto ray = inv.rays[gid];
    auto rand = Rand(gid + inv.batchStartRayIndex, inv.numRaysTotal, inv.randomSeed);

    // Iterate through all bounces
    int numEvents = 0;
    for (int i = 0; i < inv.maxEvents; numEvents = ++i) {
        // the ray might finalize due to being absorbed, or because an error occured while tracing!
        if (!isRayActive(ray.m_eventType)) break;

        Collision col = findCollision(i, inv.sequential, ray, inv.elements, inv.numElements, rand);
        if (!col.found) {
            // no element was hit.
            // Tracing is done!
            break;
        }

        // transform ray and intersection point in ELEMENT coordiantes
        const auto element = inv.elements[col.elementIndex];
        ray = rayMatrixMult(ray, element.m_inTrans);

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
            case BehaveType::ImagePlane:
                ray = behaveImagePlane(ray);
                break;
        }

        // write ray in local element coordinates to global memory
        outputEvents.events[gid * inv.maxEvents + i] = ray;

        // transform back to WORLD coordinates
        ray = rayMatrixMult(ray, element.m_outTrans);
    }

    // store recorded events count
    outputEvents.eventCounts[gid] = numEvents;
}

}  // namespace RAYX
