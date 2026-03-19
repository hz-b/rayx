#include "Device.h"

#include <algorithm>
#include <alpaka/alpaka.hpp>
#include <ranges>
#include <sstream>

#include "rayx/IO/Debug.h"
#include "rayx/IO/Exception.h"
#include "rayx/IO/Instrumentor.h"

namespace rayx {

/// factory for creating Device instances, only used within rayx-core, not exposed to the user
struct DeviceFactory {
    static Device createDevice(DeviceType type, std::string name, Device::Score score, Device::Index indexWithinDeviceType) {
        return Device(type, std::move(name), score, indexWithinDeviceType);
    }
};

namespace {

template <typename AccTag>
std::vector<Device> fetchAvailableDevicesForDeviceType(const DeviceType deviceType) {
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
        const auto score = static_cast<Device::Score>(props.m_multiProcessorCount);
        const auto index = static_cast<Device::Index>(i);

        const auto device = DeviceFactory::createDevice(
            deviceType,
            alpaka::getName(dev),
            score,
            index
        );

        devices.push_back(device);
    }

    return devices;
}

}  // unnamed namespace

std::string toString(const DeviceType deviceType) {
    switch (deviceType) {
        case DeviceType::CpuSerial:
            return "CpuSerial";
        case DeviceType::CpuParallel:
            return "CpuParallel";
        case DeviceType::GpuCuda:
            return "GpuCuda";
        default:
            throw detail::unimplementedCodePathException(std::format("unknown device type: {}", static_cast<int>(deviceType)));
    }
}

std::vector<Device> fetchAvailableDevices(DeviceTypeMask mask) {
    auto devices = std::vector<Device>();

    auto append = [&devices]<typename AccTag>(const DeviceType deviceType, const AccTag) mutable {
        auto platformDevices = fetchAvailableDevicesForDeviceType<AccTag>(deviceType);
        devices.insert(devices.end(), platformDevices.begin(), platformDevices.end());
    };

    if (!!(mask & DeviceTypeMask::CpuSerial))
#ifdef RAYX_CPU_SEQ_ENABLED
        append(DeviceType::CpuSerial, alpaka::TagCpuSerial{});
#else
        std::cout << "device type CpuSerial is requested but was not enabled during build, skipping.";
#endif

    if (!!(mask & DeviceTypeMask::CpuParallel))
#ifdef RAYX_CPU_OMP_ENABLED
        append(DeviceType::CpuParallel, alpaka::TagCpuOmp2Blocks{});
#else
        std::cout << "device type CpuParallel is requested but was not enabled during build, skipping.";
#endif

    if (!!(mask & DeviceTypeMask::GpuCuda))
#ifdef RAYX_GPU_CUDA_ENABLED
        append(DeviceType::GpuCuda, alpaka::AccGpuCudaRt{});
#else
        std::cout << "device type GpuCuda is requested but was not enabled during build, skipping.";
#endif

    return devices;
}

Device getBestDeviceByScore(const std::vector<Device>& devices) {
    if (devices.empty()) throw std::invalid_argument("error: devices list must not be empty");

    const Device* bestDevice = &devices.front();

    for (const auto& device : devices) {
        if (device.score() > bestDevice->score()) bestDevice = &device;
    }

    return *bestDevice;
}

void dumpDevices(const std::vector<Device>& devices) {
    std::cout << "Number of available devices: " << devices.size();
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& device = devices[i];
        std::cout << "Device - index: " << i << ", type: " << toString(device.type()) << ", score" << device.score() << ", name: " << device.name();
    }
}

}  // namespace rayx
