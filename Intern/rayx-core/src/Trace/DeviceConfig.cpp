#include "DeviceConfig.h"

#include <algorithm>
#include <alpaka/alpaka.hpp>
#include <ranges>
#include <sstream>

#include "IO/Debug.h"
#include "IO/Instrumentor.h"

namespace {

using DeviceType = rayx::trace::DeviceConfig::DeviceType;
using Device     = rayx::trace::DeviceConfig::Device;
using Index      = Device::Index;
using Score      = Device::Score;

template <typename Platform>
DeviceType platformToDeviceType();

template <>
DeviceType platformToDeviceType<alpaka::PlatformCpu>() {
#if defined(RAYX_OPENMP_ENABLED)
    return DeviceType::CpuParallel;
#else
    return DeviceType::CpuSerial;
#endif
}

#if defined(RAYX_CUDA_ENABLED)
template <>
DeviceType platformToDeviceType<alpaka::PlatformCudaRt>() {
    return DeviceType::GpuCuda;
}
#endif

template <typename AccTag>
std::vector<Device> getAvailableDevicesProps() {
    std::vector<Device> devices;

    using Dim      = alpaka::DimInt<1>;
    using Idx      = int32_t;
    using Acc      = alpaka::TagToAcc<AccTag, Dim, Idx>;
    using Platform = alpaka::Platform<Acc>;

    const auto platform   = Platform{};
    const auto numDevices = alpaka::getDevCount(platform);

    for (size_t i = 0; i < numDevices; ++i) {
        const auto dev   = alpaka::getDevByIdx(platform, i);
        const auto props = alpaka::getAccDevProps<Acc>(dev);
        const auto index = static_cast<Index>(i);
        const auto score = static_cast<Score>(props.m_multiProcessorCount);

        const auto device = Device{
            .type   = platformToDeviceType<Platform>(),
            .name   = alpaka::getName(dev),
            .index  = index,
            .score  = score,
            .enable = false,
        };

        devices.push_back(device);
    }

    return devices;
}

std::vector<Device> getAvailableDevices(DeviceType deviceType = DeviceType::All) {
    RAYX_PROFILE_SCOPE_STDOUT("initDevices");

    auto devices = std::vector<Device>();

    auto append = [&devices]<typename AccTag>(const AccTag&) mutable {
        auto platformDevices = getAvailableDevicesProps<AccTag>();
        devices.insert(devices.end(), platformDevices.begin(), platformDevices.end());
    };

#if defined(RAYX_OPENMP_ENABLED)
    if (deviceType & DeviceType::CpuParallel) append(alpaka::TagCpuOmp2Blocks{});
#else
    if (deviceType & DeviceType::CpuSerial) append(alpaka::TagCpuSerial{});
#endif

#if defined(RAYX_CUDA_ENABLED)
    if (deviceType & DeviceType::GpuCuda) append(alpaka::TagGpuCudaRt{});
#endif

    return devices;
}

std::string deviceTypeToString(DeviceType deviceType) {
    std::vector<const char*> names;

    if (deviceType & DeviceType::CpuSerial) names.push_back("CpuSerial");
    if (deviceType & DeviceType::CpuParallel) names.push_back("CpuParallel");
    if (deviceType & DeviceType::GpuCuda) names.push_back("GpuCuda");

    if (names.empty()) names.push_back("None");

    std::stringstream ss;

    // join names with separator
    for (size_t i = 0; i < names.size(); ++i) {
        if (i != 0) ss << " | ";
        ss << names[i];
    }

    return ss.str();
}

}  // unnamed namespace

namespace rayx::trace {

DeviceConfig::DeviceConfig(DeviceType fetchedDeviceType) : devices(getAvailableDevices(fetchedDeviceType)), m_fetchedDeviceType(fetchedDeviceType) {}

void DeviceConfig::dumpDevices() const {
    RAYX_LOG << "Number of available devices: " << devices.size();
    RAYX_LOG << "Fetched device types: " << deviceTypeToString(m_fetchedDeviceType);
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& device = devices[i];
        RAYX_LOG << "Device - index: " << i << ", type: " << deviceTypeToString(device.type) << ", name: " << device.name;
    }

#if !defined(RAYX_CUDA_ENABLED)
    if (!(m_fetchedDeviceType & DeviceType::Cpu)) RAYX_WARN << "GPU support is not enabled in this build.";
#endif
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
        if (device.type & deviceType) { filteredDevices.push_back(&device); }
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

DeviceConfig::DeviceType DeviceConfig::availableDeviceTypes() {
    DeviceType deviceType = DeviceType::None;

#if defined(RAYX_OPENMP_ENABLED)
    deviceType = static_cast<DeviceType>(deviceType | DeviceType::CpuParallel);
#else
    deviceType = static_cast<DeviceType>(deviceType | DeviceType::CpuSerial);
#endif

#if defined(RAYX_CUDA_ENABLED)
    deviceType = static_cast<DeviceType>(deviceType | DeviceType::GpuCuda);
#endif

    return deviceType;
}

}  // namespace rayx
