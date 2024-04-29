// TODO(Sven):
// can there be multiple cpu devices? like on a multi cpu system
// allow multiple devices in DeviceConfig
// one cpu + multiple gpu ??

#include <ranges>
#include <algorithm>
#include <sstream>
#include <experimental/iterator>

#include <Debug/Debug.h>

#include "Platform.h"
#include "DeviceConfig.h"

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

#if defined(RAYX_CUDA)
template <>
DeviceType platformToDeviceType<alpaka::PlatformCudaRt>() {
    return DeviceType::GpuCuda;
}
#endif

#if defined(RAYX_HIP)
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

#if defined(RAYX_CUDA)
template <typename Dim, typename Idx>
struct AccForPlatform<alpaka::PlatformCudaRt, Dim, Idx> {
    using type = alpaka::AccGpuCudaRt<Dim, Idx>;
};
#endif

#if defined(RAYX_HIP)
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

        const auto device = Device {
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

    auto append = [&devices] (const auto platform) mutable {
        auto platformDevices = getAvailableDevicesForPlatform(platform);
        devices.insert(devices.end(), platformDevices.begin(), platformDevices.end());
    };

    if (deviceType & DeviceType::Cpu)
        append(alpaka::PlatformCpu());

#if defined(RAYX_CUDA)
    if (deviceType & DeviceType::GpuCuda)
        append(alpaka::PlatformCudaRt());
#endif

#if defined(RAYX_HIP)
    if (deviceType & DeviceType::GpuHip)
        append(alpaka::PlatformHipRt());
#endif

    return devices;
}

std::string deviceTypeToString(DeviceType deviceType) {
    std::vector<std::string> names;

    if (deviceType & DeviceType::Cpu)
        names.push_back("Cpu");
    if (deviceType & DeviceType::GpuCuda)
        names.push_back("GpuCuda");
    if (deviceType & DeviceType::GpuHip)
        names.push_back("GpuHip");

    if (names.empty())
        return "Unsupported";

    std::stringstream ss;
    std::copy(names.begin(), names.end(), std::experimental::ostream_joiner<std::string>(ss, " | "));

    return ss.str();
}

} // unnamed namespace

namespace RAYX {

DeviceConfig::DeviceConfig(DeviceType fetchedDeviceType) :
    devices(getAvailableDevices(fetchedDeviceType))
{
}

void DeviceConfig::dumpDevices() const {
    RAYX_LOG << "Available devices: " << devices.size();
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& device = devices[i];
        RAYX_LOG << "Device - index: " << i << ", type: " << deviceTypeToString(device.type) << ", name: " << device.name;
    }
}

size_t DeviceConfig::enabledDevicesCount() const {
    size_t count = 0;
    for (const auto& device : devices)
        count += device.enable ? 1 : 0;
    return count;
}

DeviceConfig& DeviceConfig::enableAllDevices(DeviceType deviceType) {
    for (auto& device : devices)
        device.enable = device.type | deviceType;

    return *this;
}

DeviceConfig& DeviceConfig::enableBestDevice(DeviceType deviceType) {
    auto compare_score = [] (
        const Device& a,
        const Device& b
    ) {
        return a.score < b.score;
    };

    auto devicesByTypeView = devices | std::views::filter([deviceType] (const Device& device) {
        return device.type & deviceType;
    });
    auto bestIt = std::ranges::max_element(devicesByTypeView, compare_score);

    if (bestIt == devicesByTypeView.end()) {
        dumpDevices();
        RAYX_WARN
            << "Could not find best device for types: " << deviceTypeToString(deviceType);
    }

    bestIt->enable = true;
    return *this;
}

DeviceConfig& DeviceConfig::enableDeviceByIndex(const Device::Index deviceIndex) {
    if (devices.size() <= deviceIndex) {
        dumpDevices();
        RAYX_ERR << "Specified device index is out of range: " << deviceIndex;
    }

    devices[deviceIndex].enable = true;
    return *this;
}

} // namespace RAYX
