#include "Trace.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
void traceSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);

    storeRay(getRecordIndex(gid, 0, constState.outputEventsGridStride), mutableState.storedFlags, mutableState.events, ray,
             constState.objectRecordMask, ray.object_id, constState.attrRecordMask);

    rayMatrixMult(constState.objectTransforms[ray.object_id].m_inTrans, ray.position, ray.direction, ray.electric_field);

    for (int elementIndex = 0; elementIndex < constState.numElements; ++elementIndex) {
        if (isRayTerminated(ray.event_type)) break;

        const auto element = constState.elements[elementIndex];

        rayMatrixMult(constState.objectTransforms[elementIndex + constState.numSources].m_inTrans, ray.position, ray.direction, ray.electric_field);

        const auto col = findCollisionInElementCoords(ray.position, ray.direction, element, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        const auto col_optical_distance = glm::length(ray.position - col->hitpoint);
        ray.optical_path_length += col_optical_distance;
        ray.electric_field = advanceElectricField(ray.electric_field, energyToWaveLength(ray.energy), col_optical_distance);
        ray.path_event_id  = elementIndex;
        ray.position       = col->hitpoint;
        ray.object_id      = constState.numSources + elementIndex;
        ray.event_type     = EventType::HitElement;

        behave(ray, *col, element, constState.materials);

        storeRay(getRecordIndex(gid, ray.object_id, constState.outputEventsGridStride), mutableState.storedFlags, mutableState.events, ray,
                 constState.objectRecordMask, ray.object_id, constState.attrRecordMask);

        rayMatrixMult(constState.objectTransforms[elementIndex + constState.numSources].m_outTrans, ray.position, ray.direction, ray.electric_field);
    }
}

RAYX_FN_ACC
void traceNonSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);

    storeRay(getRecordIndex(gid, 0, constState.outputEventsGridStride), mutableState.storedFlags, mutableState.events, ray,
             constState.objectRecordMask, ray.object_id, constState.attrRecordMask);

    rayMatrixMult(constState.objectTransforms[ray.object_id].m_inTrans, ray.position, ray.direction, ray.electric_field);

    for (int hitIndex = 0; hitIndex < constState.maxEvents; ++hitIndex) {
        if (isRayTerminated(ray.event_type)) break;

        const auto col = findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.objectTransforms, constState.numSources, constState.numElements, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        const auto element = constState.elements[col->elementIndex];
        rayMatrixMult(constState.objectTransforms[col->elementIndex + constState.numSources].m_inTrans, ray.position, ray.direction, ray.electric_field);

        const auto col_optical_distance = glm::length(ray.position - col->point.hitpoint);
        ray.optical_path_length += col_optical_distance;
        ray.electric_field = advanceElectricField(ray.electric_field, energyToWaveLength(ray.energy), col_optical_distance);
        ray.path_event_id  = hitIndex;
        ray.position       = col->point.hitpoint;
        ray.object_id      = constState.numSources + col->elementIndex;
        ray.event_type     = EventType::HitElement;

        behave(ray, col->point, element, constState.materials);

        // check if the number of events exceed capacity. if so, set event type to TooManyEvents
        if (hitIndex == constState.maxEvents - 1 && !isRayTerminated(ray.event_type)) {
            // still something to hit?
            if (findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.objectTransforms, constState.numSources, constState.numElements, ray.rand))
                ray.event_type = EventType::TooManyEvents;
        }

        const auto recordIndex = hitIndex + 1;  // add 1 because the source rays have been stored already
        storeRay(getRecordIndex(gid, recordIndex, constState.outputEventsGridStride), mutableState.storedFlags, mutableState.events, ray,
                 constState.objectRecordMask, ray.object_id, constState.attrRecordMask);

        rayMatrixMult(constState.objectTransforms[col->elementIndex + constState.numSources].m_outTrans, ray.position, ray.direction, ray.electric_field);
    }
}

}  // namespace RAYX
