#pragma once

#include <string>
#include <vector>

namespace RAYX {

struct RAYX_API DeviceConfig {
    enum RAYX_API DeviceType {
        Unsupported = 0,
        Cpu         = 1 << 0,
        GpuCuda     = 1 << 2,
        GpuHip      = 1 << 3,
        Gpu         = GpuCuda | GpuHip,
        All         = Cpu | Gpu,
    };

    struct RAYX_API Device {
        using Index = size_t;
        using Score = size_t;

        DeviceType type;
        std::string name;
        Index index;
        Score score;
        bool enable;
    };

    DeviceConfig(DeviceType fetchedDeviceType = DeviceType::All);
    DeviceConfig(const DeviceConfig&) = default;
    DeviceConfig(DeviceConfig&&) = default;

    DeviceConfig& operator= (const DeviceConfig&) = default;
    DeviceConfig& operator= (DeviceConfig&&) = default;

    void dumpDevices() const;
    size_t enabledDevicesCount() const;

    DeviceConfig& enableAllDevices(DeviceType deviceType = DeviceType::All);
    DeviceConfig& enableBestDevice(DeviceType deviceType = DeviceType::All);
    DeviceConfig& enableDeviceByIndex(const Device::Index deviceIndex);

    std::vector<Device> devices;
};

} // namespace RAYX
