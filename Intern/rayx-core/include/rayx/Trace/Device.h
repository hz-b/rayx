#pragma once

#include <string>
#include <vector>

#include "rayx/Core.h"

namespace rayx {

enum class DeviceType {
    CpuSerial,
    CpuParallel,
    GpuCuda,
};

std::string toString(const DeviceType deviceType);

enum class DeviceTypeMask {
    None        = 0,
    CpuSerial   = 1 << static_cast<int>(DeviceType::CpuSerial),
    CpuParallel = 1 << static_cast<int>(DeviceType::CpuParallel),
    Cpu         = CpuSerial | CpuParallel,
    GpuCuda     = 1 << static_cast<int>(DeviceType::GpuCuda),
    Gpu         = GpuCuda,
    All         = Cpu | Gpu,
};

constexpr inline DeviceTypeMask operator|(const DeviceTypeMask lhs, const DeviceTypeMask rhs) {
    return static_cast<DeviceTypeMask>(static_cast<std::underlying_type_t<DeviceTypeMask>>(lhs) |
                                       static_cast<std::underlying_type_t<DeviceTypeMask>>(rhs));
}
constexpr inline DeviceTypeMask operator&(const DeviceTypeMask lhs, const DeviceTypeMask rhs) {
    return static_cast<DeviceTypeMask>(static_cast<std::underlying_type_t<DeviceTypeMask>>(lhs) &
                                       static_cast<std::underlying_type_t<DeviceTypeMask>>(rhs));
}
constexpr inline DeviceTypeMask operator^(const DeviceTypeMask lhs, const DeviceTypeMask rhs) {
    return static_cast<DeviceTypeMask>(static_cast<std::underlying_type_t<DeviceTypeMask>>(lhs) ^
                                       static_cast<std::underlying_type_t<DeviceTypeMask>>(rhs));
}
constexpr inline DeviceTypeMask operator~(const DeviceTypeMask lhs) {
    return static_cast<DeviceTypeMask>(~static_cast<std::underlying_type_t<DeviceTypeMask>>(lhs)) & DeviceTypeMask::All;
}
constexpr inline bool operator!(const DeviceTypeMask lhs) { return lhs == DeviceTypeMask::None; }
constexpr inline DeviceTypeMask& operator|=(DeviceTypeMask& lhs, const DeviceTypeMask rhs) { return lhs = lhs | rhs; }
constexpr inline DeviceTypeMask& operator&=(DeviceTypeMask& lhs, const DeviceTypeMask rhs) { return lhs = lhs & rhs; }
constexpr inline DeviceTypeMask& operator^=(DeviceTypeMask& lhs, const DeviceTypeMask rhs) { return lhs = lhs ^ rhs; }

class RAYX_API Device {
    // only DeviceFactory can create Device instances
    // there is no reason for the user to create instances of Device, as only rayx-core can fetch device information consistently
    friend struct DeviceFactory;
    friend struct Tracer;

  public:
    using Score = size_t;
    using Index = size_t;

    DeviceType type() const { return m_type; }
    const std::string& name() const { return m_name; }
    Score score() const { return m_score; }

  private:
    // private ctor to ensure that only DeviceFactory can create Device instances
    Device(DeviceType type, std::string name, Score score, Index indexWithinDeviceType)
        : m_type(type), m_name(std::move(name)), m_score(score), m_indexWithinDeviceType(indexWithinDeviceType) {};

    DeviceType m_type;
    std::string m_name;
    Score m_score;
    Index m_indexWithinDeviceType;  // index of the device among devices of the same type. This is necessary to create the corresponding Alpaka
                                    // accelerator.
};

std::vector<Device> fetchAvailableDevices(DeviceTypeMask mask = DeviceTypeMask::All);
Device getBestDeviceByScore(const std::vector<Device>& devices);
void dumpDevices(const std::vector<Device>& devices);

}  // namespace rayx
