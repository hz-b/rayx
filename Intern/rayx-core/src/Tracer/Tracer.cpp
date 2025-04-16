#include "Tracer.h"

#include <algorithm>

#include "MegaKernelTracer.h"

namespace {

using DeviceType = RAYX::DeviceConfig::DeviceType;
using DeviceIndex = RAYX::DeviceConfig::Device::Index;

inline std::shared_ptr<RAYX::DeviceTracer> createDeviceTracer(DeviceType deviceType, DeviceIndex deviceIndex) {
    switch (deviceType) {
        case DeviceType::GpuCuda:
#if defined(RAYX_CUDA_ENABLED)
            return std::make_shared<RAYX::MegaKernelTracer<alpaka::TagGpuCudaRt>>(deviceIndex);
#else
            RAYX_EXIT << "Failed to create Tracer with Cuda device. Cuda was disabled during build.";
            return nullptr;
#endif
        case DeviceType::GpuHip:
#if defined(RAYX_HIP_ENABLED)
            eturn std::make_shared<RAYX::MegaKernelTracer<alpaka::TagGpuHipRt>>(deviceIndex);
#else
            RAYX_EXIT << "Failed to create Tracer with Hip device. Hip was disabled during build.";
            return nullptr;
#endif
        default: // case DeviceType::Cpu
#if defined(RAYX_OPENMP_ENABLED)
            using TagCpu = alpaka::TagCpuOmp2Blocks;
#else
            using TagCpu = alpaka::TagCpuSerial;
#endif
            return std::make_shared<RAYX::MegaKernelTracer<TagCpu>>(deviceIndex);
    }
}

}  // unnamed namespace

namespace RAYX {

Tracer::Tracer(const DeviceConfig& deviceConfig) {
    if (deviceConfig.enabledDevicesCount() != 1) RAYX_EXIT << "The number of selected devices must be exactly 1!";

    for (const auto& device : deviceConfig.devices) {
        if (device.enable) {
            RAYX_VERB << "Creating tracer with device: " << device.name;
            m_deviceTracer = createDeviceTracer(device.type, device.index);
            break;
        }
    }
}

BundleHistory Tracer::trace(const Group& group, Sequential sequential, uint64_t max_batch_size, int THREAD_COUNT, uint32_t maxEvents) {
    // in sequential tracing, maxEvents should be equal to the number of elements
    if (sequential == Sequential::Yes)
        maxEvents = group.numElements();

    return m_deviceTracer->trace(group, sequential, max_batch_size, THREAD_COUNT, maxEvents);
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

int Tracer::defaultMaxEvents(const Group* group) {
    if (group) return group->numElements() * 2 + 8;
    return 32;
}

}  // namespace RAYX
