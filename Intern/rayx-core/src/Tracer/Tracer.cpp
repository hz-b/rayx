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
        default:  // case DeviceType::Cpu
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

RaySoA Tracer::trace(const Group& group, Sequential sequential, uint64_t maxBatchSize, uint32_t maxEvents, const std::vector<bool>& recordMask, const RayAttrFlag attr) {
    // in sequential tracing, maxEvents should be equal to the number of elements
    if (sequential == Sequential::Yes) maxEvents = group.numElements();

    return m_deviceTracer->trace(group, sequential, static_cast<int>(maxBatchSize), static_cast<int>(maxEvents), recordMask, attr);
}

int Tracer::defaultMaxEvents(const Group* group) {
    if (group) return group->numElements() * 2 + 8;
    return 32;
}

}  // namespace RAYX
