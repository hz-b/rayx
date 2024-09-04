#pragma once

#include <string>
#include <vector>

#include "Core.h"

namespace RAYX {

struct RAYX_API DeviceConfig {
    enum RAYX_API DeviceType {
        Unsupported = 0,         //
        Cpu = 1 << 0,            //
        GpuCuda = 1 << 1,        //
        GpuHip = 1 << 2,         //
        Gpu = GpuCuda | GpuHip,  //
        All = Cpu | Gpu,         //
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

    DeviceConfig& operator=(const DeviceConfig&) = default;
    DeviceConfig& operator=(DeviceConfig&&) = default;

    void dumpDevices() const;
    size_t enabledDevicesCount() const;

    DeviceConfig& disableAllDevices(DeviceType deviceType = DeviceType::All);
    DeviceConfig& enableAllDevices(DeviceType deviceType = DeviceType::All);

    DeviceConfig& disableDeviceByIndex(const Device::Index deviceIndex);
    DeviceConfig& enableDeviceByIndex(const Device::Index deviceIndex);

    DeviceConfig& enableBestDevice(DeviceType deviceType = DeviceType::All);

    std::vector<Device> devices;

  private:
    DeviceType m_fetchedDeviceType;
};

}  // namespace RAYX