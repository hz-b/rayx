#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
Ray loadRay(const int i, const Rays& __restrict rays) {
    return {
        .path_id             = rays.path_id[i],
        .path_event_id       = rays.path_event_id[i],
        .position            = rays.position(i),
        .event_type          = rays.event_type[i],
        .direction           = rays.direction(i),
        .energy              = rays.energy[i],
        .electric_field      = rays.electric_field(i),
        .optical_path_length = rays.optical_path_length[i],
        .order               = rays.order[i],
        .object_id           = rays.object_id[i],
        .source_id           = rays.source_id[i],
        .rand                = Rand(rays.rand_counter[i]),
    };
}

RAYX_FN_ACC
void storeRay(const int i, Rays& __restrict rays, const Ray& __restrict ray, const bool* __restrict elementRecordMask, const int elementIndex,
              const RayAttrFlag attrRecordMask, bool* __restrict storedFlag) {
    // element record mask
    if (!recordMasks.elementRecordMask[elementIndex]) return;

    // event type record mask
    // TODO: this should be done on path level
    // if ((recordMasks.eventTypeRecordMask & EventTypeMask::Success) == EventTypeMask::None) return;

    // attribute record mask
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::PathId)) rays.path_id[i] = ray.path_id;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::PathEventId)) rays.path_event_id[i] = ray.path_event_id;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::PositionX)) rays.position_x[i] = ray.position.x;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::PositionY)) rays.position_y[i] = ray.position.y;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::PositionZ)) rays.position_z[i] = ray.position.z;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::EventType)) rays.event_type[i] = ray.event_type;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::DirectionX)) rays.direction_x[i] = ray.direction.x;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::DirectionY)) rays.direction_y[i] = ray.direction.y;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::DirectionZ)) rays.direction_z[i] = ray.direction.z;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::Energy)          rays.energy[i]           = ray.energy;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::ElectricFieldX)) rays.electric_field_x[i] = ray.electric_field.x;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::ElectricFieldY)) rays.electric_field_y[i] = ray.electric_field.y;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::ElectricFieldZ)) rays.electric_field_z[i] = ray.electric_field.z;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::OpticalPathLength)) rays.optical_path_length[i] = ray.optical_path_length;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::Order)) rays.order[i] = ray.order;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::ObjectId)) rays.object_id[i] = ray.object_id;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::SourceId)) rays.source_id[i] = ray.source_id;
    if (RayAttrFlag::None != (attrRecordMask & RayAttrFlag::RandCounter)) rays.rand_counter[i] = ray.rand.counter;

    // mark as stored
    storedFlag[i] = true;
}

RAYX_FN_ACC
int getRecordIndex(int gid, int numRecorded, int gridStride) { return gid + numRecorded * gridStride; }

RAYX_FN_ACC
void traceSequential(const int gid, const ConstState& __restrict constState, MutableState& __restrict mutableState) {
    auto ray = loadRay(gid, constState.rays);

    for (int elementIndex = 0; elementIndex < constState.numElements; ++elementIndex) {
        if (ray.event_type & EventType) break;

        const auto element = constState.elements[elementIndex];
        rayMatrixMult(element.m_inTrans, ray.position, ray.direction, ray.electric_field);

        const auto col = findCollisionInElementCoords(ray.position, ray.direction, element, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        ray.path_event_id = elementIndex;
        ray.optical_path_length += glm::length(ray.position - col->hitpoint);
        ray.position     = col->hitpoint;
        ray.object_index = constState.numSources + elementIndex;
        ray.event_type   = EventType::HitElement;

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
        if (isRayTerminated(ray.event_type)) break;

        const auto col = findCollisionWithElements(ray.position, ray.direction, constState.elements, constState.numElements, ray.rand);

        // no element was hit. tracing is done!
        if (!col) break;

        const auto element = constState.elements[col->elementIndex];
        rayMatrixMult(element.m_inTrans, ray.position, ray.direction, ray.electric_field);

        ray.path_event_id = hitIndex;
        ray.optical_path_length += glm::length(ray.position - col->hitpoint);
        ray.position     = col->hitpoint;
        ray.object_index = constState.numSources + elementIndex;
        ray.event_type   = EventType::HitElement;

        behave(ray, *col, element, constState.materials);

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
