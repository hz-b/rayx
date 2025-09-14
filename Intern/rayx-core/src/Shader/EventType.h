#pragma once

#include "Core.h"
#include "Throw.h"

namespace RAYX {

// TODO: doc this enum and all its members
enum class EventType {
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
RAYX_FN_ACC inline void terminateRay(Ray& __restrict ray, const EventType eventType) {
    _debug_warn(!isRayTerminated(ray.event_type), "ray about to be terminated, but ray is already terminated!");
    _debug_assert(isRayTerminated(eventType), "ray about to be terminated, but provided event type (%d) is not a valid termination event type!",
                  static_cast<int>(eventType));
    ray.m_event_type = eventType;
}

enum class EventTypeMask {
    Uninitialized = 1 << static_cast<int>(EventType::Uninitialized),
    Emitted       = 1 << static_cast<int>(EventType::Emitted),
    HitElement    = 1 << static_cast<int>(EventType::HitElement),
    FatalError    = 1 << static_cast<int>(EventType::FatalError),
    Absorbed      = 1 << static_cast<int>(EventType::Absorbed),
    BeyondHorizon = 1 << static_cast<int>(EventType::BeyondHorizon),
    TooManyEvents = 1 << static_cast<int>(EventType::TooManyEvents),
    None          = 0,
};

RAYX_FN_ACC constexpr inline EventTypeMask operator|(const EventTypeMask lhs, const EventTypeMask rhs) {
    return static_cast<EventTypeMask>(static_cast<std::underlying_type<EventTypeMask>>(lhs) & static_cast<std::underlying_type<EventTypeMask>>(rhs));
}
RAYX_FN_ACC constexpr inline EventTypeMask operator&(const EventTypeMask lhs, const EventTypeMask rhs) {
    return static_cast<EventTypeMask>(static_cast<std::underlying_type<EventTypeMask>>(lhs) & static_cast<std::underlying_type<EventTypeMask>>(rhs));
}
RAYX_FN_ACC constexpr inline EventTypeMask operator^(const EventTypeMask lhs, const EventTypeMask rhs) {
    return static_cast<EventTypeMask>(static_cast<std::underlying_type<EventTypeMask>>(lhs) ^ static_cast<std::underlying_type<EventTypeMask>>(rhs));
}
RAYX_FN_ACC constexpr inline EventTypeMask operator~(const EventTypeMask lhs) {
    return static_cast<EventTypeMask>(~static_cast<std::underlying_type<EventTypeMask>>(lhs));
}
RAYX_FN_ACC constexpr inline EventTypeMask operator|=(EventTypeMask& lhs, const EventTypeMask rhs) { return lhs = lhs | rhs; }
RAYX_FN_ACC constexpr inline EventTypeMask operator&=(EventTypeMask& lhs, const EventTypeMask rhs) { return lhs = lhs & rhs; }
RAYX_FN_ACC constexpr inline EventTypeMask operator^=(EventTypeMask& lhs, const EventTypeMask rhs) { return lhs = lhs ^ rhs; }

}  // namespace RAYX
