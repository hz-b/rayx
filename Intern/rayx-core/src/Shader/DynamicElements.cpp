#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
void dynamicElements(const int gid, const ConstState& constState, MutableState& mutableState) {
    auto ray = constState.rays[gid];

    int numRecorded = 0;
    if (constState.recordMask.shouldRecordSource(ray.m_sourceID)) {
        recordEvent(mutableState.events, ray, getRecordIndex(gid, numRecorded, constState.maxEvents));
        ++numRecorded;
    }

    auto rand = std::move(mutableState.rands[gid]);

    // Iterate through all bounces
    bool colNotFound = false;
    for (int bounce = 0; bounce < constState.maxEvents; ++bounce) {
        // the ray might finalize due to being absorbed, or because an error occured while tracing!
        if (!isRayActive(ray.m_eventType)) break;

        Collision col =
            findCollision(bounce, constState.sequential, ray.m_position, ray.m_direction, constState.elements, constState.numElements, rand);
        if (!col.found) {
            // no element was hit.
            // Tracing is done!
            colNotFound = true;
            break;
        }

        // transform ray and intersection point in ELEMENT coordiantes
        const auto element = constState.elements[col.elementIndex];
        ray = rayMatrixMult(element.m_inTrans, ray);

        // Calculate interaction(reflection,material, absorption etc.) of ray with detected next element
        const auto behaviour = element.m_behaviour;

        ray.m_pathLength += glm::length(ray.m_position - col.hitpoint);
        ray.m_position = col.hitpoint;
        ray.m_lastElement = col.elementIndex;
        ray.m_eventType = EventType::HitElement;

        switch (behaviour.m_type) {
            case BehaveType::Mirror:
                ray = behaveMirror(ray, col, element.m_material, constState.materialIndices, constState.materialTables);
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
                ray = behaveFoil(ray, behaviour, col, element.m_material, constState.materialIndices, constState.materialTables);
                break;
        }

        // write ray in local element coordinates to global memory
        if (numRecorded < constState.maxEvents && (constState.recordMask.shouldRecordElement(col.elementIndex))) {
            recordEvent(mutableState.events, ray, getRecordIndex(gid, numRecorded, constState.maxEvents));
            ++numRecorded;
        }

        // transform back to WORLD coordinates
        ray = rayMatrixMult(element.m_outTrans, ray);
    }

    // check if the number of events exceeds capacity
    if (!colNotFound && constState.sequential == Sequential::No && isRayActive(ray.m_eventType)) {
        Collision col = findCollisionNonSequential(ray.m_position, ray.m_direction, constState.elements, constState.numElements, rand);
        if (col.found && (constState.recordMask.shouldRecordElement(col.elementIndex))) {
            ray = terminateRay(ray, EventType::TooManyEvents);
            recordEvent(mutableState.events, ray, getRecordIndex(gid, numRecorded, constState.maxEvents));
        }
    }

    // store recorded events count
    mutableState.eventCounts[gid] = numRecorded;
}

}  // namespace RAYX
