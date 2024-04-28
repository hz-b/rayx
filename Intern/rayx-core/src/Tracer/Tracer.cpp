#include "Tracer.h"

#include <algorithm>

#include "Platform.h"
#include "SimpleTracer.h"

namespace {

using DeviceType = RAYX::DeviceConfig::DeviceType;
using DeviceIndex = RAYX::DeviceConfig::Device::Index;

inline std::shared_ptr<RAYX::DeviceTracer> createDeviceTracer(DeviceType deviceType, DeviceIndex deviceIndex) {
    using Dim = alpaka::DimInt<1>;
    using Idx = int32_t;

    switch (deviceType) {
    case DeviceType::GpuCuda:
#if defined(RAYX_CUDA)
        using GpuAccCuda = RAYX::GpuAccCuda<Dim, Idx>;
        return std::make_shared<RAYX::SimpleTracer<GpuAccCuda>>(deviceIndex);
#else
        RAYX_ERR << "Failed to create Tracer with Cuda device. Cuda was disabled during build.";
        return nullptr;
#endif
    case DeviceType::GpuHip:
#if defined(RAYX_HIP)
        using GpuAccHip = RAYX::GpuAccHip<Dim, Idx>;
        return std::make_shared<RAYX::SimpleTracer<GpuAccHip>>(deviceIndex);
#else
        RAYX_ERR << "Failed to create Tracer with Hip device. Hip was disabled during build.";
        return nullptr;
#endif
    default: // case DeviceType::Cpu
        using CpuAcc = RAYX::DefaultCpuAcc<Dim, Idx>;
        return std::make_shared<RAYX::SimpleTracer<CpuAcc>>(deviceIndex);
    }
}

} // unnamed namespace

namespace RAYX {

Tracer::Tracer(const DeviceConfig& deviceConfig) {
    if (deviceConfig.enabledDevicesCount() != 1)
        RAYX_ERR << "The number of selected devices must be exactly 1!";

    for (const auto& device : deviceConfig.devices) {
        if (device.enable) {
            RAYX_LOG << "Creating tracer with device: " << device.name;
            m_deviceTracer = createDeviceTracer(device.type, device.index);
            break;
        }
    }
}

BundleHistory Tracer::trace(
    const Beamline& beamline,
    Sequential sequential,
    uint64_t max_batch_size,
    int THREAD_COUNT,
    unsigned int maxEvents,
    int startEventID
) {
    return m_deviceTracer->trace(
        beamline,
        sequential,
        max_batch_size,
        THREAD_COUNT,
        maxEvents,
        startEventID
    );
}

/// Get the last event for each ray of the bundle.
std::vector<Ray> extractLastEvents(const BundleHistory& hist) {
    std::vector<Ray> out;
    for (auto& ray_hist : hist) {
        out.push_back(ray_hist.back());
    }

    return out;
}

BundleHistory convertToBundleHistory(const std::vector<Ray>& rays) {
    BundleHistory out;
    for (auto r : rays) {
        out.push_back({r});
    }
    return out;
}

}  // namespace RAYX
