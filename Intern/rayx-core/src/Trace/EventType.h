#pragma once

#include <cstdint>

// TODO
// #include "Throw.h"

namespace rayx::trace {

// TODO: doc this enum and all its members
enum class EventType : uint32_t {
    Uninitialized = 0,
    Emitted       = 1,
    HitElement    = 2,
    FatalError    = 3,
    Absorbed      = 4,
    BeyondHorizon = 5,
    TooManyEvents = 6,
};

RAYX_FN_ACC inline bool isRayTerminated(const EventType eventType) {
    return !(eventType == EventType::Emitted || eventType == EventType::HitElement);
}

RAYX_FN_ACC inline void terminateRay(EventType& __restrict dstEventType, const EventType srcEventType) {
    // TODO: enable
    // _debug_warn(!isRayTerminated(dstEventType), "ray about to be terminated, but ray is already terminated!");
    // _debug_assert(isRayTerminated(srcEventType), "ray about to be terminated, but provided event type (%d) is not a
    // valid termination event type!",
    //               static_cast<int>(srcEventType));
    dstEventType = srcEventType;
}

// TODO
// enum class EventTypeMask : std::underlying_type_t<EventType> {
//     None          = 0,
//     Uninitialized = 1 << static_cast<int>(EventType::Uninitialized),
//     Emitted       = 1 << static_cast<int>(EventType::Emitted),
//     HitElement    = 1 << static_cast<int>(EventType::HitElement),
//     FatalError    = 1 << static_cast<int>(EventType::FatalError),
//     Absorbed      = 1 << static_cast<int>(EventType::Absorbed),
//     BeyondHorizon = 1 << static_cast<int>(EventType::BeyondHorizon),
//     TooManyEvents = 1 << static_cast<int>(EventType::TooManyEvents),
// };
//
// RAYX_FN_ACC constexpr inline EventTypeMask operator|(const EventTypeMask lhs, const EventTypeMask rhs) {
//     return static_cast<EventTypeMask>(static_cast<std::underlying_type_t<EventTypeMask>>(lhs) &
//                                       static_cast<std::underlying_type_t<EventTypeMask>>(rhs));
// }
// RAYX_FN_ACC constexpr inline EventTypeMask operator&(const EventTypeMask lhs, const EventTypeMask rhs) {
//     return static_cast<EventTypeMask>(static_cast<std::underlying_type_t<EventTypeMask>>(lhs) &
//                                       static_cast<std::underlying_type_t<EventTypeMask>>(rhs));
// }
// RAYX_FN_ACC constexpr inline EventTypeMask operator^(const EventTypeMask lhs, const EventTypeMask rhs) {
//     return static_cast<EventTypeMask>(static_cast<std::underlying_type_t<EventTypeMask>>(lhs) ^
//                                       static_cast<std::underlying_type_t<EventTypeMask>>(rhs));
// }
// RAYX_FN_ACC constexpr inline EventTypeMask operator~(const EventTypeMask lhs) {
//     return static_cast<EventTypeMask>(~static_cast<std::underlying_type_t<EventTypeMask>>(lhs));
// }
// RAYX_FN_ACC constexpr inline bool operator!(const EventTypeMask lhs) { return lhs == EventTypeMask::None; }
//
// RAYX_FN_ACC constexpr inline EventTypeMask operator|=(EventTypeMask& lhs, const EventTypeMask rhs) { return lhs = lhs
// | rhs; } RAYX_FN_ACC constexpr inline EventTypeMask operator&=(EventTypeMask& lhs, const EventTypeMask rhs) { return
// lhs = lhs & rhs; } RAYX_FN_ACC constexpr inline EventTypeMask operator^=(EventTypeMask& lhs, const EventTypeMask rhs)
// { return lhs = lhs ^ rhs; }
//
// RAYX_FN_ACC constexpr inline EventTypeMask eventTypeToMask(const EventType eventType) {
//     return static_cast<EventTypeMask>(std::underlying_type_t<EventTypeMask>(1) <<
//     static_cast<std::underlying_type_t<EventTypeMask>>(eventType));
// }

}  // namespace rayx::trace
