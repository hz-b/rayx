#pragma once

#include <vector>

#include "Core.h"
#include "Shader/Ray.h"

#ifdef RAYX_X
#error 'X' must not be defined at this point
#endif

// clang-format off
#define RAYX_X_MACRO_RAY_ATTR_PATH_ID     X(int32_t,          path_id,          PathId,         _             )
#define RAYX_X_MACRO_RAY_ATTR_POSITION_X  X(double,           position_x,       PositionX,      m_position.x  )
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Y  X(double,           position_y,       PositionY,      m_position.y  )
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Z  X(double,           position_z,       PositionZ,      m_position.z  )
#define RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE  X(EventType,        event_type,       EventType,      m_eventType   )
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_X X(double,           direction_x,      DirectionX,     m_direction.x )
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y X(double,           direction_y,      DirectionY,     m_direction.y )
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z X(double,           direction_z,      DirectionZ,     m_direction.z )
#define RAYX_X_MACRO_RAY_ATTR_ENERGY      X(double,           energy,           Energy,         m_energy      )
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_X     X(complex::Complex, electric_field_x, ElectricFieldX, m_field.x     )
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Y     X(complex::Complex, electric_field_y, ElectricFieldY, m_field.y     )
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Z     X(complex::Complex, electric_field_z, ElectricFieldZ, m_field.z     )
#define RAYX_X_MACRO_RAY_ATTR_PATH_LENGTH X(double,           path_length,      PathLength,     m_pathLength  )
#define RAYX_X_MACRO_RAY_ATTR_ORDER       X(Order,            order,            Order,          m_order       )
#define RAYX_X_MACRO_RAY_ATTR_ELEMENT_ID  X(ElementId,        element_id,       ElementId,      m_lastElement )
#define RAYX_X_MACRO_RAY_ATTR_SOURCE_ID   X(SourceId,         source_id,        SourceId,       m_sourceID    )
// clang-format on

#define RAYX_X_MACRO_RAY_ATTR_MAPPED       \
    RAYX_X_MACRO_RAY_ATTR_POSITION_X       \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Y       \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Z       \
    RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE       \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_X      \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y      \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z      \
    RAYX_X_MACRO_RAY_ATTR_ENERGY           \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_X \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Y \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Z \
    RAYX_X_MACRO_RAY_ATTR_PATH_LENGTH      \
    RAYX_X_MACRO_RAY_ATTR_ORDER            \
    RAYX_X_MACRO_RAY_ATTR_ELEMENT_ID       \
    RAYX_X_MACRO_RAY_ATTR_SOURCE_ID

#define RAYX_X_MACRO_RAY_ATTR     \
    RAYX_X_MACRO_RAY_ATTR_PATH_ID \
    RAYX_X_MACRO_RAY_ATTR_MAPPED

#define RAYX_X_MACRO_RAY_ATTR_BUT_ELECTRIC_FIELD \
    RAYX_X_MACRO_RAY_ATTR_PATH_ID                \
    RAYX_X_MACRO_RAY_ATTR_POSITION_X             \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Y             \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Z             \
    RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE             \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_X            \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y            \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z            \
    RAYX_X_MACRO_RAY_ATTR_ENERGY                 \
    RAYX_X_MACRO_RAY_ATTR_PATH_LENGTH            \
    RAYX_X_MACRO_RAY_ATTR_ORDER                  \
    RAYX_X_MACRO_RAY_ATTR_ELEMENT_ID             \
    RAYX_X_MACRO_RAY_ATTR_SOURCE_ID

#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_X   \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Y   \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Z

namespace RAYX {

using RayAttrFlagType = uint32_t;
enum class RayAttrFlag : RayAttrFlagType {
    // clang-format off
    PathId         = 1 << 0,
    PositionX      = 1 << 1,
    PositionY      = 1 << 2,
    PositionZ      = 1 << 3,
    EventType      = 1 << 4,
    DirectionX     = 1 << 5,
    DirectionY     = 1 << 6,
    DirectionZ     = 1 << 7,
    Energy         = 1 << 8,
    ElectricFieldX = 1 << 9,
    ElectricFieldY = 1 << 10,
    ElectricFieldZ = 1 << 11,
    PathLength     = 1 << 12,
    Order          = 1 << 13,
    ElementId      = 1 << 14,
    SourceId       = 1 << 15,
    RayAttrFlagCount = 16,

    Position      = PositionX | PositionY | PositionZ,
    Direction     = DirectionX | DirectionY | DirectionZ,
    ElectricField = ElectricFieldX | ElectricFieldY | ElectricFieldZ,

    None = 0,
    All = (1 << RayAttrFlagCount) - 1,
    // clang-format on
};

constexpr inline RayAttrFlag operator|(const RayAttrFlag lhs, const RayAttrFlag rhs) {
    return static_cast<RayAttrFlag>(static_cast<RayAttrFlagType>(lhs) | static_cast<RayAttrFlagType>(rhs));
}
constexpr inline RayAttrFlag operator&(const RayAttrFlag lhs, const RayAttrFlag rhs) {
    return static_cast<RayAttrFlag>(static_cast<RayAttrFlagType>(lhs) & static_cast<RayAttrFlagType>(rhs));
}
constexpr inline RayAttrFlag operator^(const RayAttrFlag lhs, const RayAttrFlag rhs) {
    return static_cast<RayAttrFlag>(static_cast<RayAttrFlagType>(lhs) ^ static_cast<RayAttrFlagType>(rhs));
}
constexpr inline RayAttrFlag operator|=(RayAttrFlag& lhs, const RayAttrFlag rhs) { return lhs = lhs | rhs; }
constexpr inline RayAttrFlag operator&=(RayAttrFlag& lhs, const RayAttrFlag rhs) { return lhs = lhs & rhs; }
constexpr inline RayAttrFlag operator^=(RayAttrFlag& lhs, const RayAttrFlag rhs) { return lhs = lhs ^ rhs; }
constexpr inline RayAttrFlag operator~(const RayAttrFlag lhs) { return static_cast<RayAttrFlag>(~static_cast<RayAttrFlagType>(lhs)); }

struct RaySoA {
    int num_events;
    int num_paths;
    RayAttrFlag attr;

#define X(type, name, flag, map) std::vector<type> name;

    RAYX_X_MACRO_RAY_ATTR
#undef X

    glm::dvec3 position(int i) const { return glm::dvec3(position_x[i], position_y[i], position_z[i]); }
    glm::dvec3 direction(int i) const { return glm::dvec3(direction_x[i], direction_y[i], direction_z[i]); }
    ElectricField field(int i) const { return ElectricField(electric_field_x[i], electric_field_y[i], electric_field_z[i]); }
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
