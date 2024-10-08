// TODO(Sven):
// can there be multiple cpu devices? like on a multi cpu system
// allow multiple devices in DeviceConfig
// one cpu + multiple gpu ??

#include "DeviceConfig.h"

#include <algorithm>
#include <ranges>
#include <sstream>

#include "Debug/Debug.h"
#include "Platform.h"

namespace {

using DeviceType = RAYX::DeviceConfig::DeviceType;
using Device = RAYX::DeviceConfig::Device;
using Index = Device::Index;
using Score = Device::Score;

template <typename Platform>
DeviceType platformToDeviceType();

template <>
DeviceType platformToDeviceType<alpaka::PlatformCpu>() {
    return DeviceType::Cpu;
}

#if defined(RAYX_CUDA_ENABLED)
template <>
DeviceType platformToDeviceType<alpaka::PlatformCudaRt>() {
    return DeviceType::GpuCuda;
}
#endif

#if defined(RAYX_HIP_ENABLED)
template <>
DeviceType platformToDeviceType<alpaka::PlatformHipRt>() {
    return DeviceType::GpuHip;
}
#endif

template <typename Platform, typename Dim, typename Idx>
struct AccForPlatform;

template <typename Dim, typename Idx>
struct AccForPlatform<alpaka::PlatformCpu, Dim, Idx> {
    using type = RAYX::DefaultCpuAcc<Dim, Idx>;
};

#if defined(RAYX_CUDA_ENABLED)
template <typename Dim, typename Idx>
struct AccForPlatform<alpaka::PlatformCudaRt, Dim, Idx> {
    using type = alpaka::AccGpuCudaRt<Dim, Idx>;
};
#endif

#if defined(RAYX_HIP_ENABLED)
template <typename Dim, typename Idx>
struct AccForPlatform<alpaka::PlatformHipRt, Dim, Idx> {
    using type = alpaka::AccGpuHipRt<Dim, Idx>;
};
#endif

template <typename Platform, typename Dim, typename Idx>
using AccForPlatform_t = typename AccForPlatform<Platform, Dim, Idx>::type;

template <typename Platform>
std::vector<Device> getAvailableDevicesForPlatform(const Platform platform) {
    std::vector<Device> devices;

    const auto count = alpaka::getDevCount(platform);
    for (size_t i = 0; i < count; ++i) {
        using Dim = alpaka::DimInt<1>;
        using Idx = int32_t;
        using Acc = AccForPlatform_t<Platform, Dim, Idx>;
        const auto dev = alpaka::getDevByIdx(platform, i);
        const auto props = alpaka::getAccDevProps<Acc>(dev);
        const auto index = static_cast<Index>(i);
        const auto score = static_cast<Score>(props.m_multiProcessorCount);

        const auto device = Device{
            .type = platformToDeviceType<Platform>(),
            .name = alpaka::getName(dev),
            .index = index,
            .score = score,
            .enable = false,
        };

        devices.push_back(device);
    }

    return devices;
}

std::vector<Device> getAvailableDevices(DeviceType deviceType = DeviceType::All) {
    auto devices = std::vector<Device>();

    auto append = [&devices](const auto platform) mutable {
        auto platformDevices = getAvailableDevicesForPlatform(platform);
        devices.insert(devices.end(), platformDevices.begin(), platformDevices.end());
    };

    if (deviceType & DeviceType::Cpu) append(alpaka::PlatformCpu());

#if defined(RAYX_CUDA_ENABLED)
    if (deviceType & DeviceType::GpuCuda) append(alpaka::PlatformCudaRt());
#endif

#if defined(RAYX_HIP_ENABLED)
    if (deviceType & DeviceType::GpuHip) append(alpaka::PlatformHipRt());
#endif

    return devices;
}

std::string deviceTypeToString(DeviceType deviceType) {
    std::vector<const char*> names;

    if (deviceType & DeviceType::Cpu) names.push_back("Cpu");
    if (deviceType & DeviceType::GpuCuda) names.push_back("GpuCuda");
    if (deviceType & DeviceType::GpuHip) names.push_back("GpuHip");

    if (names.empty()) names.push_back("Unsupported");

    std::stringstream ss;

    // join names with separator
    for (size_t i = 0; i < names.size(); ++i) {
        if (i != 0) ss << " | ";
        ss << names[i];
    }

    return ss.str();
}

}  // unnamed namespace

namespace RAYX {

DeviceConfig::DeviceConfig(DeviceType fetchedDeviceType) : devices(getAvailableDevices(fetchedDeviceType)), m_fetchedDeviceType(fetchedDeviceType) {}

void DeviceConfig::dumpDevices() const {
    RAYX_LOG << "Number of available devices: " << devices.size();
    RAYX_LOG << "Fetched device types: " << deviceTypeToString(m_fetchedDeviceType);
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& device = devices[i];
        RAYX_LOG << "Device - index: " << i << ", type: " << deviceTypeToString(device.type) << ", name: " << device.name;
    }
}

size_t DeviceConfig::enabledDevicesCount() const {
    size_t count = 0;
    for (const auto& device : devices) count += device.enable ? 1 : 0;
    return count;
}

DeviceConfig& DeviceConfig::disableAllDevices(DeviceType deviceType) {
    for (auto& device : devices) device.enable = !(device.type | deviceType);

    return *this;
}

DeviceConfig& DeviceConfig::enableAllDevices(DeviceType deviceType) {
    for (auto& device : devices) device.enable = device.type | deviceType;

    return *this;
}

DeviceConfig& DeviceConfig::disableDeviceByIndex(const Device::Index deviceIndex) {
    if (devices.size() <= deviceIndex) {
        dumpDevices();
        RAYX_EXIT << "Specified device index is out of range: " << deviceIndex;
    }

    devices[deviceIndex].enable = false;
    return *this;
}

DeviceConfig& DeviceConfig::enableDeviceByIndex(const Device::Index deviceIndex) {
    if (devices.size() <= deviceIndex) {
        dumpDevices();
        RAYX_EXIT << "Specified device index is out of range: " << deviceIndex;
    }

    devices[deviceIndex].enable = true;
    return *this;
}

DeviceConfig& DeviceConfig::enableBestDevice(DeviceType deviceType) {
    auto compare_score = [](const Device& a, const Device& b) { return a.score < b.score; };

    // Manually filter devices by type
    std::vector<Device*> filteredDevices;
    for (auto& device : devices) {
        if (device.type & deviceType) {
            filteredDevices.push_back(&device);
        }
    }

    // Use std::max_element on the filtered devices
    auto bestIt =
        std::max_element(filteredDevices.begin(), filteredDevices.end(), [compare_score](Device* a, Device* b) { return compare_score(*a, *b); });

    if (bestIt == filteredDevices.end()) {
        dumpDevices();
        RAYX_EXIT << "Could not find best device for types: " << deviceTypeToString(static_cast<DeviceType>(m_fetchedDeviceType & deviceType));
    }

    (*bestIt)->enable = true;
    return *this;
}

}  // namespace RAYX
