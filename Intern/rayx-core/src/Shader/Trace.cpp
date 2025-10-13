#include "Trace.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
#include "Utils.h"

namespace RAYX {

#define assertObjectIdInBounds(object_id, numObjects) \
    _debug_assert(0 <= object_id && object_id < numObjects, "error: ray object id '%d' is out of bounds [0, %d)", object_id, numObjects);

RAYX_FN_ACC
void traceSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);
    assertObjectIdInBounds(ray.object_id, constState.numSources + constState.numElements);
    // TODO: do we want to increment here? its a design question. in case one traces one beamline and uses events to trace another beamline, the
    // ray_path_id does not overlap, because it was incremented
    ++ray.path_event_id;

    const auto stored = storeRay(getRecordIndex(gid, 0, constState.outputEventsGridStride), mutableState.storedFlags, mutableState.events, ray,
                                 constState.objectRecordMask, ray.object_id, constState.attrRecordMask);
    ray.path_event_id += stored ? 1 : 0;

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
        ray.position = col->hitpoint;
        ray.object_id = constState.numSources + elementIndex;
        ray.event_type = EventType::HitElement;

        behave(ray, *col, element, constState.materialIndices, constState.materialTable);

        assertObjectIdInBounds(ray.object_id, constState.numSources + constState.numElements);
        const auto stored = storeRay(getRecordIndex(gid, ray.object_id, constState.outputEventsGridStride), mutableState.storedFlags,
                                     mutableState.events, ray, constState.objectRecordMask, ray.object_id, constState.attrRecordMask);
        ray.path_event_id += stored ? 1 : 0;

        rayMatrixMult(constState.objectTransforms[elementIndex + constState.numSources].m_outTrans, ray.position, ray.direction, ray.electric_field);
    }
}

RAYX_FN_ACC
void traceNonSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);
    assertObjectIdInBounds(ray.object_id, constState.numSources + constState.numElements);
    // TODO: see above (traceSequential)
    ++ray.path_event_id;

    const auto stored = storeRay(getRecordIndex(gid, 0, constState.outputEventsGridStride), mutableState.storedFlags, mutableState.events, ray,
                                 constState.objectRecordMask, ray.object_id, constState.attrRecordMask);
    ray.path_event_id += stored ? 1 : 0;

    // TODO: object_id from previous beamline is not correct for this beamline
    rayMatrixMult(constState.objectTransforms[ray.object_id].m_inTrans, ray.position, ray.direction, ray.electric_field);

    for (int hitIndex = 0; hitIndex < constState.maxEvents; ++hitIndex) {
        if (isRayTerminated(ray.event_type)) break;

        const auto col = findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.objectTransforms,
                                                   constState.numSources, constState.numElements, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        const auto element = constState.elements[col->elementIndex];
        rayMatrixMult(constState.objectTransforms[col->elementIndex + constState.numSources].m_inTrans, ray.position, ray.direction,
                      ray.electric_field);

        const auto col_optical_distance = glm::length(ray.position - col->point.hitpoint);
        ray.optical_path_length += col_optical_distance;
        ray.electric_field = advanceElectricField(ray.electric_field, energyToWaveLength(ray.energy), col_optical_distance);
        ray.position = col->point.hitpoint;
        ray.object_id = constState.numSources + col->elementIndex;
        ray.event_type = EventType::HitElement;

        behave(ray, col->point, element, constState.materialIndices, constState.materialTable);

        // check if the number of events exceed capacity. if so, set event type to TooManyEvents
        if (hitIndex == constState.maxEvents - 1 && !isRayTerminated(ray.event_type)) {
            // still something to hit?
            if (findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.objectTransforms, constState.numSources,
                                          constState.numElements, ray.rand))
                ray.event_type = EventType::TooManyEvents;
        }

        const auto recordIndex = hitIndex + 1;  // add 1 because one source event has potentially been stored already
        assertObjectIdInBounds(ray.object_id, constState.numSources + constState.numElements);
        const auto stored = storeRay(getRecordIndex(gid, recordIndex, constState.outputEventsGridStride), mutableState.storedFlags,
                                     mutableState.events, ray, constState.objectRecordMask, ray.object_id, constState.attrRecordMask);
        ray.path_event_id += stored ? 1 : 0;

        rayMatrixMult(constState.objectTransforms[col->elementIndex + constState.numSources].m_outTrans, ray.position, ray.direction,
                      ray.electric_field);
    }
}

}  // namespace RAYX
