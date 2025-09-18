#pragma once

#include "Ray.h"
#include "RaysPtr.h"

namespace RAYX {

RAYX_FN_ACC
inline int getRecordIndex(int gid, int numRecorded, int gridStride) { return gid + numRecorded * gridStride; }

RAYX_FN_ACC
inline Ray loadRay(const int i, const RaysPtr& __restrict rays) {
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
inline void storeRay(const int i, RaysPtr& __restrict rays, const Ray& __restrict ray) {
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
inline void storeRay(const int i, bool* __restrict storedFlags, RaysPtr& __restrict rays, const Ray& __restrict ray,
                     const bool* __restrict elementRecordMask, const int elementIndex, const RayAttrMask attrRecordMask) {
    // element record mask
    if (!elementRecordMask[elementIndex]) return;

    // attribute record mask
    if (!!(attrRecordMask & RayAttrMask::PathId)) rays.path_id[i] = ray.path_id;
    if (!!(attrRecordMask & RayAttrMask::PathEventId)) rays.path_event_id[i] = ray.path_event_id;
    if (!!(attrRecordMask & RayAttrMask::PositionX)) rays.position_x[i] = ray.position.x;
    if (!!(attrRecordMask & RayAttrMask::PositionY)) rays.position_y[i] = ray.position.y;
    if (!!(attrRecordMask & RayAttrMask::PositionZ)) rays.position_z[i] = ray.position.z;
    if (!!(attrRecordMask & RayAttrMask::EventType)) rays.event_type[i] = ray.event_type;
    if (!!(attrRecordMask & RayAttrMask::DirectionX)) rays.direction_x[i] = ray.direction.x;
    if (!!(attrRecordMask & RayAttrMask::DirectionY)) rays.direction_y[i] = ray.direction.y;
    if (!!(attrRecordMask & RayAttrMask::DirectionZ)) rays.direction_z[i] = ray.direction.z;
    if (!!(attrRecordMask & RayAttrMask::Energy)) rays.energy[i] = ray.energy;
    if (!!(attrRecordMask & RayAttrMask::ElectricFieldX)) rays.electric_field_x[i] = ray.electric_field.x;
    if (!!(attrRecordMask & RayAttrMask::ElectricFieldY)) rays.electric_field_y[i] = ray.electric_field.y;
    if (!!(attrRecordMask & RayAttrMask::ElectricFieldZ)) rays.electric_field_z[i] = ray.electric_field.z;
    if (!!(attrRecordMask & RayAttrMask::OpticalPathLength)) rays.optical_path_length[i] = ray.optical_path_length;
    if (!!(attrRecordMask & RayAttrMask::Order)) rays.order[i] = ray.order;
    if (!!(attrRecordMask & RayAttrMask::ObjectId)) rays.object_id[i] = ray.object_id;
    if (!!(attrRecordMask & RayAttrMask::SourceId)) rays.source_id[i] = ray.source_id;
    if (!!(attrRecordMask & RayAttrMask::RandCounter)) rays.rand_counter[i] = ray.rand.counter;

    // mark as stored
    storedFlags[i] = true;
}

}  // namespace RAYX
