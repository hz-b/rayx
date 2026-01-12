#pragma once

#include <vector>

#include "Core.h"
#include "Math/ElectricField.h"
#include "EventType.h"

// TODO
using RandCounter = uint64_t;

// TODO: all this macros should be invisible to the user
// TODO: implement template based solution instead of macros

#ifdef X
#error macro 'X' must not be defined at this point
#endif

#define RAYX_X_MACRO_RAY_ATTR_PATH_ID             X(int32_t, path_id, PathId)
#define RAYX_X_MACRO_RAY_ATTR_PATH_EVENT_ID       X(int32_t, path_event_id, PathEventId)
#define RAYX_X_MACRO_RAY_ATTR_POSITION_X          X(double, position_x, PositionX)
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Y          X(double, position_y, PositionY)
#define RAYX_X_MACRO_RAY_ATTR_POSITION_Z          X(double, position_z, PositionZ)
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_X         X(double, direction_x, DirectionX)
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Y         X(double, direction_y, DirectionY)
#define RAYX_X_MACRO_RAY_ATTR_DIRECTION_Z         X(double, direction_z, DirectionZ)
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_X    X(math::Complex, electric_field_x, ElectricFieldX)
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Y    X(math::Complex, electric_field_y, ElectricFieldY)
#define RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD_Z    X(math::Complex, electric_field_z, ElectricFieldZ)
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

namespace rayx::trace {

#define X(type, name, flag) static_assert(std::is_nothrow_move_constructible_v<type>);  // ensure efficient moves
RAYX_X_MACRO_RAY_ATTR
#undef X

/**
 * @brief Mask to specify ray attributes.
 * Each attribute is represented as a bit flag, allowing for efficient combination and checking of multiple attributes.
 * It is used to represent the various attributes that can be recorded for rays during ray tracing.
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
    ObjectId          = 1 << 14,
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
    return static_cast<RayAttrMask>(~static_cast<std::underlying_type_t<RayAttrMask>>(lhs)) & RayAttrMask::All;
}
RAYX_API RAYX_FN_ACC constexpr inline bool operator!(const RayAttrMask lhs) { return lhs == RayAttrMask::None; }
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask& operator|=(RayAttrMask& lhs, const RayAttrMask rhs) { return lhs = lhs | rhs; }
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask& operator&=(RayAttrMask& lhs, const RayAttrMask rhs) { return lhs = lhs & rhs; }
RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask& operator^=(RayAttrMask& lhs, const RayAttrMask rhs) { return lhs = lhs ^ rhs; }

/**
 * @brief Check if a RayAttrMask (haystack) contains another RayAttrMask (needle).
 * @param haystack The RayAttrMask to check.
 * @param needle The RayAttrMask to look for.
 * @return True if the haystack contains the needle, false otherwise.
 * @note When repeatedly checking on the same haystack, this function should be preferred over Rays::contains.
 */
RAYX_API RAYX_FN_ACC constexpr inline bool contains(const RayAttrMask haystack, const RayAttrMask needle) { return (haystack & needle) == needle; }

RAYX_API RAYX_FN_ACC constexpr inline RayAttrMask exclude(const RayAttrMask haystack, const RayAttrMask needle) { return haystack & ~needle; }

/**
 * @brief Count the number of set bits (i.e., attributes) in a RayAttrMask.
 * @param mask The RayAttrMask to count the set bits in.
 * @return The number of set bits in the RayAttrMask.
 */
RAYX_API int countSetBits(const RayAttrMask mask);

/**
 * @brief Check if a RayAttrMask represents a single attribute (i.e., is a flag).
 * @param attr The RayAttrMask to check.
 * @return True if the RayAttrMask represents a single attribute, false otherwise.
 */
RAYX_API bool isFlag(const RayAttrMask attr);

RAYX_API std::string to_string(const RayAttrMask attr);

/**
 * @brief Get a list of all possible ray attribute names.
 * @return A vector of strings containing all possible ray attribute names.
 */
RAYX_API std::vector<std::string> getRayAttrNames();

/**
 * @brief Convert a list of ray attribute names to a RayAttrMask.
 * @param strings A vector of strings containing ray attribute names.
 * @return A RayAttrMask representing the specified attributes.
 */
RAYX_API RayAttrMask rayAttrStringsToRayAttrMask(const std::vector<std::string>& strings);

}  // namespace rayx::trace
