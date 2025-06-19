#pragma once

#include <vector>

#include "Core.h"
#include "Shader/Ray.h"

#ifdef RAYX_X
#error 'RAYX_X' must not be defined at this point
#endif

// clang-format off
#define RAYX_X_MACRO_RAY_ATTR_PATH_ID      RAYX_X(int32_t,   path_id,      RayPathId,     _               )
#define RAYX_X_MACRO_RAY_ATTR_POSITION_X   RAYX_X(double,    position_x,   RayPositionX,  m_position.x    )
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Y   RAYX_X(double,    position_y,   RayPositionY,  m_position.y    )
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Z   RAYX_X(double,    position_z,   RayPositionZ,  m_position.z    )
#define RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE   RAYX_X(EventType, event_type,   RayEventType,  m_eventType     )
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_X  RAYX_X(double,    direction_x,  RayDirectionX, m_direction.x   )
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y  RAYX_X(double,    direction_y,  RayDirectionY, m_direction.y   )
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z  RAYX_X(double,    direction_z,  RayDirectionZ, m_direction.z   )
#define RAYX_X_MACRO_RAY_ATTR_ENERGY       RAYX_X(double,    energy,       RayEnergy,     m_energy        )
#define RAYX_X_MACRO_RAY_ATTR_FIELD_X_REAL RAYX_X(double,    field_x_real, RayFieldXReal, m_field.x.real())
#define RAYX_X_MACRO_RAY_ATTR_FIELD_X_IMAG RAYX_X(double,    field_x_imag, RayFieldXImag, m_field.x.imag())
#define RAYX_X_MACRO_RAY_ATTR_FIELD_Y_REAL RAYX_X(double,    field_y_real, RayFieldYReal, m_field.y.real())
#define RAYX_X_MACRO_RAY_ATTR_FIELD_Y_IMAG RAYX_X(double,    field_y_imag, RayFieldYImag, m_field.y.imag())
#define RAYX_X_MACRO_RAY_ATTR_FIELD_Z_REAL RAYX_X(double,    field_z_real, RayFieldZReal, m_field.z.real())
#define RAYX_X_MACRO_RAY_ATTR_FIELD_Z_IMAG RAYX_X(double,    field_z_imag, RayFieldZImag, m_field.z.imag())
#define RAYX_X_MACRO_RAY_ATTR_PATH_LENGTH  RAYX_X(double,    path_length,  RayPathLength, m_pathLength    )
#define RAYX_X_MACRO_RAY_ATTR_ORDER        RAYX_X(Order,     order,        RayOrder,      m_order         )
#define RAYX_X_MACRO_RAY_ATTR_ELEMENT_ID   RAYX_X(ElementId, element_id,   RayElementId,  m_lastElement   )
#define RAYX_X_MACRO_RAY_ATTR_SOURCE_ID    RAYX_X(SourceId,  source_id,    RaySourceId,   m_sourceID      )
// clang-format on

#define RAYX_X_MACRO_RAY_ATTR          \
    RAYX_X_MACRO_RAY_ATTR_POSITION_X   \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Y   \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Z   \
    RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE   \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_X  \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y  \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z  \
    RAYX_X_MACRO_RAY_ATTR_ENERGY       \
    RAYX_X_MACRO_RAY_ATTR_FIELD_X_REAL \
    RAYX_X_MACRO_RAY_ATTR_FIELD_X_IMAG \
    RAYX_X_MACRO_RAY_ATTR_FIELD_Y_REAL \
    RAYX_X_MACRO_RAY_ATTR_FIELD_Y_IMAG \
    RAYX_X_MACRO_RAY_ATTR_FIELD_Z_REAL \
    RAYX_X_MACRO_RAY_ATTR_FIELD_Z_IMAG \
    RAYX_X_MACRO_RAY_ATTR_PATH_LENGTH  \
    RAYX_X_MACRO_RAY_ATTR_ORDER        \
    RAYX_X_MACRO_RAY_ATTR_ELEMENT_ID   \
    RAYX_X_MACRO_RAY_ATTR_SOURCE_ID

namespace RAYX {

using RayAttrFlagType = uint32_t;
enum RayAttrFlag : RayAttrFlagType {
    // clang-format off
    RayPathId     = 1 << 0,
    RayPositionX  = 1 << 1,
    RayPositionY  = 1 << 2,
    RayPositionZ  = 1 << 3,
    RayEventType  = 1 << 4,
    RayDirectionX = 1 << 5,
    RayDirectionY = 1 << 6,
    RayDirectionZ = 1 << 7,
    RayEnergy     = 1 << 8,
    RayFieldXReal = 1 << 9,
    RayFieldXImag = 1 << 10,
    RayFieldYReal = 1 << 11,
    RayFieldYImag = 1 << 12,
    RayFieldZReal = 1 << 13,
    RayFieldZImag = 1 << 14,
    RayPathLength = 1 << 15,
    RayOrder      = 1 << 16,
    RayElementId  = 1 << 17,
    RaySourceId   = 1 << 18,

    EndOfRayAttrBits = 19,
    EndOfRayAttr  = 1 << EndOfRayAttrBits,

    RayPosition   = RayPositionX | RayPositionY | RayPositionZ,
    RayDirection  = RayDirectionX | RayDirectionY | RayDirectionZ,
    RayField      = RayFieldXReal | RayFieldXImag | RayFieldYReal | RayFieldYImag | RayFieldZReal | RayFieldZImag,
    AllRayAttr    = EndOfRayAttr - 1,
    // clang-format on
};

struct RaySoA {
    int num_events;
    int num_paths;
    RayAttrFlagType attr;

#define RAYX_X(type, name, flag, map) std::vector<type> name;

    RAYX_X_MACRO_RAY_ATTR_PATH_ID
    RAYX_X_MACRO_RAY_ATTR
#undef RAYX_X

    glm::dvec3 position(int i) const { return glm::dvec3(position_x[i], position_y[i], position_z[i]); }
    glm::dvec3 direction(int i) const { return glm::dvec3(direction_x[i], direction_y[i], direction_z[i]); }
    ElectricField field(int i) const {
        return ElectricField({field_x_real[i], field_x_imag[i]}, {field_y_real[i], field_y_imag[i]}, {field_z_real[i], field_z_imag[i]});
    }
    Ray ray(int i) const {
        return Ray{
            .m_position = position(i),
            .m_eventType = event_type[i],
            .m_direction = direction(i),
            .m_energy = energy[i],
            .m_field = field(i),
            .m_pathLength = path_length[i],
            .m_order = order[i],
            .m_lastElement = element_id[i],
            .m_sourceID = source_id[i],
        };
    }
};

static_assert(std::is_nothrow_default_constructible_v<RaySoA>);

/// Contains all the events of a single Ray in chronological order.
using RayHistory = std::vector<Ray>;

/// Contains all events for some bundle of rays.
/// Given a `BundleHistory hist;`
/// hist[i][j] is the j'th event of the i'th ray of the bundle.
using BundleHistory = std::vector<RayHistory>;

RAYX_API RaySoA bundleHistoryToRaySoA(const BundleHistory& bundle);
RAYX_API BundleHistory raySoAToBundleHistory(const RaySoA& rays);

}  // namespace RAYX
