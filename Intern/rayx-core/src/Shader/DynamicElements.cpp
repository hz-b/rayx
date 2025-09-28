#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
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
        const auto coating = element.m_coating;

        ray.m_pathLength += glm::length(ray.m_position - col.hitpoint);
        ray.m_position = col.hitpoint;
        ray.m_lastElement = col.elementIndex;
        ray.m_eventType = EventType::HitElement;

        ray = variant::visit(
            [&](auto&& arg) -> Ray {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Behaviour::Mirror>) {
                    return behaveMirror(ray, col, coating, element.m_material, inv.materialIndices, inv.materialTables);
                } else if constexpr (std::is_same_v<T, Behaviour::Grating>) {
                    return behaveGrating(ray, behaviour, col);
                } else if constexpr (std::is_same_v<T, Behaviour::Slit>) {
                    return behaveSlit(ray, behaviour, rand);
                } else if constexpr (std::is_same_v<T, Behaviour::RZP>) {
                    return behaveRZP(ray, behaviour, col, rand);
                } else if constexpr (std::is_same_v<T, Behaviour::Crystal>) {
                    return behaveCrystal(ray, behaviour, col);
                } else if constexpr (std::is_same_v<T, Behaviour::ImagePlane>) {
                    return behaveImagePlane(ray);
                } else if constexpr (std::is_same_v<T, Behaviour::Foil>) {
                    return behaveFoil(ray, behaviour, col, element.m_material, inv.materialIndices, inv.materialTables);
                } else {
                    _throw("invalid behaviour type in dynamicElements!");
                    return ray;
                }
            },
            behaviour.m_behaviour);

        // write ray in local element coordinates to global memory
        if (numRecorded < inv.maxEvents && (!inv.recordMask || inv.recordMask[col.elementIndex])) {
            recordEvent(outputEvents.events, ray, getRecordIndex(gid, numRecorded, inv.maxEvents));
            ++numRecorded;
        }

        // transform back to WORLD coordinates
        ray = rayMatrixMult(element.m_outTrans, ray);
    }

    // check if the number of events exceeds capacity
    if (!colNotFound && inv.sequential == Sequential::No && isRayActive(ray.m_eventType)) {
        Collision col = findCollisionNonSequential(ray.m_position, ray.m_direction, inv.elements, inv.numElements, rand);
        if (col.found && (!inv.recordMask || inv.recordMask[col.elementIndex])) {
            ray = terminateRay(ray, EventType::TooManyEvents);
            recordEvent(outputEvents.events, ray, getRecordIndex(gid, inv.maxEvents, inv.maxEvents - 1));
        }
    }

    // store recorded events count
    outputEvents.eventCounts[gid] = numRecorded;
}

}  // namespace RAYX