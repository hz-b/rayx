#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
void traceSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);

    for (int elementIndex = 0; elementIndex < constState.numElements; ++elementIndex) {
        // TODO: check if collision detection might write to ray.event_type. then this check should be after collision detection
        // if the ray has been terminated, then tracing is done.
        // also accounts for rays that have been terminated before tracing started
        if (isRayTerminated(ray.event_type)) break;

        const auto element = constState.elements[elementIndex];
        rayMatrixMult(element.m_inTrans, ray.position, ray.direction, ray.electric_field);

        const auto col = findCollisionInElementCoords(ray.position, ray.direction, element, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        const auto col_distance = glm::length(ray.position - col->hitpoint);
        ray.optical_path_length += col_distance;
        ray.electric_field = advanceElectricField(ray.electric_field, ray.direction, col_distance);
        ray.path_event_id  = elementIndex;
        ray.position       = col->hitpoint;
        ray.object_index   = constState.numSources + elementIndex;
        ray.event_type     = EventType::HitElement;

        behave(ray, *col, element, constState.materials);

        storeRay(getRecordIndex(gid, elementIndex, constState.gridStride), mutableState.events, ray, constState.recordMasks, mutableState.storedFlag);

        rayMatrixMult(element.m_outTrans, ray.position, ray.direction, ray.electric_field);
    }
}

RAYX_FN_ACC
void traceNonSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);

    for (int hitIndex = 0; hitIndex < constState.maxEvents; ++hitIndex) {
        // TODO: check if collision detection might write to ray.event_type. then this check should be after collision detection
        // if the ray has been terminated, then tracing is done.
        // also accounts for rays that have been terminated before tracing started
        if (isRayTerminated(ray.event_type)) break;

        const auto col = findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.numElements, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        const auto element = constState.elements[col->elementIndex];
        rayMatrixMult(element.m_inTrans, ray.position, ray.direction, ray.electric_field);

        const auto col_distance = glm::length(ray.position - col->hitpoint);
        ray.optical_path_length += col_distance;
        ray.electric_field = advanceElectricField(ray.electric_field, ray.direction, col_distance);
        ray.path_event_id  = hitIndex;
        ray.position       = col->hitpoint;
        ray.object_index   = constState.numSources + elementIndex;
        ray.event_type     = EventType::HitElement;

        behave(ray, col->point, element, constState.materials);

        // check if the number of events exceed capacity. if so, set event type to TooManyEvents
        if (hitIndex == constState.maxEvents - 1 && !isRayTerminated(ray.event_type)) {
            // still something to hit?
            if (findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.numElements, ray.rand))
                ray.event_type = EventType::TooManyEvents;
        }

        storeRay(getRecordIndex(gid, elementIndex, constState.gridStride), mutableState.events, ray, constState.recordMasks, mutableState.storedFlag);

        rayMatrixMult(element.m_outTrans, ray.position, ray.direction, ray.electric_field);
    }
}

}  // namespace RAYX
