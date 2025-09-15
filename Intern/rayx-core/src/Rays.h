#pragma once

#include <vector>

#include "Core.h"
#include "Shader/ElectricField.h"
#include "Shader/EventType.h"
#include "Shader/Rand.h"

// TODO: all this macros should be invisible to the user

#ifdef RAYX_X
#error 'X' must not be defined at this point
#endif

#define RAYX_X_MACRO_RAY_ATTR_PATH_ID       X(int32_t, path_id, PathId)
#define RAYX_X_MACRO_RAY_ATTR_PATH_EVENT_ID X(int32_t, path_event_id, PathEventId)
#define RAYX_X_MACRO_RAY_ATTR_POSITION_X    X(double, position_x, PositionX)
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Y    X(double, position_y, PositionY)
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Z    X(double, position_z, PositionZ)
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_X   X(double, direction_x, DirectionX)
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y   X(double, direction_y, DirectionY)
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z   X(double, direction_z, DirectionZ)
// TODO; this should be std::complex<double>, but our ubuntu CLI did not yet catch up to support std::complex in cuda device code
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_X    X(complex::Complex, electric_field_x, ElectricFieldX)
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Y    X(complex::Complex, electric_field_y, ElectricFieldY)
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Z    X(complex::Complex, electric_field_z, ElectricFieldZ)
#define RAYX_X_MACRO_RAY_ATTR_OPTICAL_PATH_LENGTH X(double, optical_path_length, OpticalPathLength)
#define RAYX_X_MACRO_RAY_ATTR_ENERGY              X(double, energy, Energy)
#define RAYX_X_MACRO_RAY_ATTR_ORDER               X(int32_t, order, Order)
#define RAYX_X_MACRO_RAY_ATTR_OBJECT_ID           X(int32_t, object_id, ObjectId)
#define RAYX_X_MACRO_RAY_ATTR_SOURCE_ID           X(int32_t, source_id, SourceId)
#define RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE          X(EventType, event_type, EventType)
#define RAYX_X_MACRO_RAY_ATTR_RAND_COUNTER        X(RandCounter, rand_counter, RandCounter)

#define RAYX_X_MACRO_RAY_ATTR                 \
    RAYX_X_MACRO_RAY_ATTR_PATH_ID             \
    RAYX_X_MACRO_RAY_ATTR_PATH_EVENT_ID       \
    RAYX_X_MACRO_RAY_ATTR_POSITION_X          \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Y          \
    RAYX_X_MACRO_RAY_ATTR_POSITION_Z          \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_X         \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y         \
    RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z         \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_X    \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Y    \
    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Z    \
    RAYX_X_MACRO_RAY_ATTR_OPTICAL_PATH_LENGTH \
    RAYX_X_MACRO_RAY_ATTR_ENERGY              \
    RAYX_X_MACRO_RAY_ATTR_ORDER               \
    RAYX_X_MACRO_RAY_ATTR_OBJECT_ID           \
    RAYX_X_MACRO_RAY_ATTR_SOURCE_ID           \
    RAYX_X_MACRO_RAY_ATTR_EVENT_TYPE          \
    RAYX_X_MACRO_RAY_ATTR_RAND_COUNTER

namespace RAYX {

#define X(type, name, flag) static_assert(std::is_nothrow_move_constructible_v<type>);  // ensure efficient moves

RAYX_X_MACRO_RAY_ATTR
#undef X

/** @brief Mask to specify ray attributes
 * It is used to configure which attributes are recorded during ray tracing.
 */
enum class RAYX_API RayAttrMask : uint32_t {
    PathId            = 1 << 0,
    PathEventId       = 1 << 1,
    PositionX         = 1 << 2,
    PositionY         = 1 << 3,
    PositionZ         = 1 << 4,
    DirectionX        = 1 << 5,
    DirectionY        = 1 << 6,
    DirectionZ        = 1 << 7,
    ElectricFieldX    = 1 << 8,
    ElectricFieldY    = 1 << 9,
    ElectricFieldZ    = 1 << 10,
    OpticalPathLength = 1 << 11,
    Energy            = 1 << 12,
    Order             = 1 << 13,
    ObjectId         = 1 << 14,
    SourceId          = 1 << 15,
    EventType         = 1 << 16,
    RandCounter       = 1 << 17,
    RayAttrMaskCount  = 18,

    Position      = PositionX | PositionY | PositionZ,
    Direction     = DirectionX | DirectionY | DirectionZ,
    ElectricField = ElectricFieldX | ElectricFieldY | ElectricFieldZ,

    None = 0,
    All  = (1 << RayAttrMaskCount) - 1,
};

RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator|(const RayAttrMask lhs, const RayAttrMask rhs) {
    return static_cast<RayAttrMask>(static_cast<std::underlying_type_t<RayAttrMask>>(lhs) | static_cast<std::underlying_type_t<RayAttrMask>>(rhs));
}
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator&(const RayAttrMask lhs, const RayAttrMask rhs) {
    return static_cast<RayAttrMask>(static_cast<std::underlying_type_t<RayAttrMask>>(lhs) & static_cast<std::underlying_type_t<RayAttrMask>>(rhs));
}
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator^(const RayAttrMask lhs, const RayAttrMask rhs) {
    return static_cast<RayAttrMask>(static_cast<std::underlying_type_t<RayAttrMask>>(lhs) ^ static_cast<std::underlying_type_t<RayAttrMask>>(rhs));
}
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator~(const RayAttrMask lhs) {
    return static_cast<RayAttrMask>(~static_cast<std::underlying_type_t<RayAttrMask>>(lhs));
}
RAYX_API RAYX_FN_ACC constexpr inline bool operator!(const RayAttrMask lhs) { return lhs == RayAttrMask::None; }
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator|=(RayAttrMask& lhs, const RayAttrMask rhs) { return lhs = lhs | rhs; }
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator&=(RayAttrMask& lhs, const RayAttrMask rhs) { return lhs = lhs & rhs; }
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask operator^=(RayAttrMask& lhs, const RayAttrMask rhs) { return lhs = lhs ^ rhs; }

/** @brief Struct to hold ray attributes
 * Each attribute is stored in a separate vector, allowing for efficient access and manipulation of ray data.
 */
struct RAYX_API Rays {
    int num_events;
    RayAttrMask attr;

#define X(type, name, flag) std::vector<type> name;

    RAYX_X_MACRO_RAY_ATTR
#undef X

    glm::dvec3 position(const int i) const { return glm::dvec3(position_x[i], position_y[i], position_z[i]); }
    void position(const int i, const glm::dvec3 position) {
        position_x[i] = position.x;
        position_y[i] = position.y;
        position_z[i] = position.z;
    }

    glm::dvec3 direction(const int i) const { return glm::dvec3(direction_x[i], direction_y[i], direction_z[i]); }
    void direction(const int i, const glm::dvec3 direction) {
        direction_x[i] = direction.x;
        direction_y[i] = direction.y;
        direction_z[i] = direction.z;
    }

    ElectricField electric_field(const int i) const { return ElectricField(electric_field_x[i], electric_field_y[i], electric_field_z[i]); }
    void electric_field(const int i, const ElectricField electric_field) {
        electric_field_x[i] = electric_field.x;
        electric_field_y[i] = electric_field.y;
        electric_field_z[i] = electric_field.z;
    }
};

/// get a full list of ray attribute names
RAYX_API std::vector<std::string> getRayAttrNames();
/// convert list of ray attribute names to RayAttrMask
RAYX_API RayAttrMask rayAttrStringsToRayAttrMask(const std::vector<std::string>& strings);

}  // namespace RAYX
