#pragma once

#include "Ray.h"
#include "Rays.h"

namespace RAYX {

RAYX_FN_ACC
int getRecordIndex(int gid, int numRecorded, int gridStride) { return gid + numRecorded * gridStride; }

RAYX_FN_ACC
Ray loadRay(const int i, const RaysPtr& __restrict rays) {
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
void storeRay(const int i, RaysPtr& __restrict rays, const Ray& __restrict ray) {
    rays.path_id[i]             = ray.path_id;
    rays.path_event_id[i]       = ray.path_event_id;
    rays.position_x[i]          = ray.position.x;
    rays.position_y[i]          = ray.position.y;
    rays.position_z[i]          = ray.position.z;
    rays.event_type[i]          = ray.event_type;
    rays.direction_x[i]         = ray.direction.x;
    rays.direction_y[i]         = ray.direction.y;
    rays.direction_z[i]         = ray.direction.z;
    rays.energy[i]              = ray.energy;
    rays.electric_field_x[i]    = ray.electric_field.x;
    rays.electric_field_y[i]    = ray.electric_field.y;
    rays.electric_field_z[i]    = ray.electric_field.z;
    rays.optical_path_length[i] = ray.optical_path_length;
    rays.order[i]               = ray.order;
    rays.object_id[i]           = ray.object_id;
    rays.source_id[i]           = ray.source_id;
    rays.rand_counter[i]        = ray.rand.counter;
}

RAYX_FN_ACC
void storeRay(const int i, RaysPtr& __restrict rays, const Ray& __restrict ray, const bool* __restrict elementRecordMask, const int elementIndex,
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

}  // namespace RAYX
