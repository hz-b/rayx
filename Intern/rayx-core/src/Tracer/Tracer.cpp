#include "Tracer.h"

#include <algorithm>

#include "MegaKernelTracer.h"

namespace {

using DeviceType  = RAYX::DeviceConfig::DeviceType;
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

int defaultMaxEvents(const RAYX::ObjectMask& mask) { return mask.numObjectsToRecord() * 2 + 8; }

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

Rays Tracer::trace(const Group& group, const Sequential sequential, const ObjectMask& objectRecordMask, const RayAttrMask attrRecordMask,
                   std::optional<int> maxEvents, std::optional<int> maxBatchSize) {
    if (static_cast<int>(group.numSources()) != objectRecordMask.numSources() ||
        static_cast<int>(group.numElements()) != objectRecordMask.numElements()) {
        RAYX_EXIT << "Group and ObjectMask do not match! group has " << group.numSources() << " sources and " << group.numElements()
                  << " elements, but ObjectMask has " << objectRecordMask.numSources() << " sources and " << objectRecordMask.numElements()
                  << " elements.";
    }

    const auto actualMaxEvents =
        // in sequential mode maxEvents will be the same as the number of objects to record
        sequential == Sequential::Yes ? objectRecordMask.numObjectsToRecord()
                                      // in non-sequential mode maxEvents is optional, if not set, it will be estimated
                                      : (maxEvents ? *maxEvents : defaultMaxEvents(objectRecordMask));

    const auto actualMaxBatchSize = maxBatchSize ? *maxBatchSize : DEFAULT_BATCH_SIZE;

    return m_deviceTracer->trace(group, sequential, objectRecordMask, attrRecordMask, actualMaxEvents, actualMaxBatchSize);
}

}  // namespace RAYX
