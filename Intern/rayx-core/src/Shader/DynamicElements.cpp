#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "RecordEvent.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
Ray loadRay(const int i, const Rays& __restrict rays) {
    return {
        .position            = rays.position(i),
        .direction           = rays.direction(i),
        .energy              = rays.energy[i],
        .optical_path_length = rays.optical_path_length[i],
        .electric_field      = rays.electric_field(i),
        .rand                = Rand(rays.rand_counter[i]),
        .path_id             = rays.path_id[i],
        .path_event_id       = rays.path_event_id[i],
        .order               = rays.order[i],
        .object_id           = rays.object_id[i],
        .source_id           = rays.source_id[i],
        .event_type          = rays.event_type[i],
    };
}

RAYX_FN_ACC
void storeRay(const int i, Rays& __restrict rays, const Ray& __restrict ray, const bool* __restrict elementRecordMask, const int elementIndex,
              const RayAttrMask attrRecordMask, bool* __restrict storedFlag) {
    // element record mask
    if (!recordMasks.elementRecordMask[elementIndex]) return;

    // event type record mask
    // TODO: this should be done on path level
    // if ((recordMasks.eventTypeRecordMask & EventTypeMask::Success) == EventTypeMask::None) return;

    // attribute record mask
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::PathId)) rays.path_id[i] = ray.path_id;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::PathEventId)) rays.path_event_id[i] = ray.path_event_id;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::PositionX)) rays.position_x[i] = ray.position.x;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::PositionY)) rays.position_y[i] = ray.position.y;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::PositionZ)) rays.position_z[i] = ray.position.z;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::EventType)) rays.event_type[i] = ray.event_type;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::DirectionX)) rays.direction_x[i] = ray.direction.x;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::DirectionY)) rays.direction_y[i] = ray.direction.y;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::DirectionZ)) rays.direction_z[i] = ray.direction.z;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::Energy)          rays.energy[i]           = ray.energy;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::ElectricFieldX)) rays.electric_field_x[i] = ray.electric_field.x;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::ElectricFieldY)) rays.electric_field_y[i] = ray.electric_field.y;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::ElectricFieldZ)) rays.electric_field_z[i] = ray.electric_field.z;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::OpticalPathLength)) rays.optical_path_length[i] = ray.optical_path_length;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::Order)) rays.order[i] = ray.order;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::ObjectId)) rays.object_id[i] = ray.object_id;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::SourceId)) rays.source_id[i] = ray.source_id;
    if (RayAttrMask::None != (attrRecordMask & RayAttrMask::RandCounter)) rays.rand_counter[i] = ray.rand.counter;

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
