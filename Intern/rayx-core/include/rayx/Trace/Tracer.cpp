#include "Tracer.h"

#include <algorithm>
#include <format>

#include "Device.h"
#include "MegaKernelTracer.h"
#include "rayx/Design/Beamline.h"
#include "rayx/IO/Exception.h"
#include "rayx/Model/ToModel/Beamline.h"

namespace rayx {

namespace {

[[maybe_unused]] std::exception disabledDeviceTypeException(DeviceType deviceType) {
    return std::runtime_error(std::format(
        "disabled device type: {}. the device type has not been enabled during build, thus cannot be used during runtime.", toString(deviceType)));
}

std::shared_ptr<detail::DeviceTracer> createDeviceTracer(DeviceType deviceType, Device::Index deviceIndex) {
    switch (deviceType) {
        case DeviceType::GpuCuda:
#if defined(RAYX_CPU_SEQ_ENABLED)
            return std::make_shared<detail::MegaKernelTracer<alpaka::TagCpuSerial>>(deviceIndex);
#else
            throw disabledDeviceTypeException(deviceType);
#endif
        case DeviceType::CpuSerial:
#if defined(RAYX_CPU_OMP_ENABLED)
            return std::make_shared<detail::MegaKernelTracer<alpaka::TagCpuOmp2Blocks>>(deviceIndex);
#else
            throw disabledDeviceTypeException(deviceType);
#endif
        case DeviceType::CpuParallel:
#if defined(RAYX_GPU_CUDA_ENABLED)
            return std::make_shared<detail::MegaKernelTracer<alpaka::AccGpuCudaRt>>(deviceIndex);
#else
            throw disabledDeviceTypeException(deviceType);
#endif
        default:
            throw detail::unimplementedCodePathException(std::format("unknown device type: {}", static_cast<int>(deviceType)));
    }
}

}  // unnamed namespace

Tracer::Tracer() : Tracer(getBestDeviceByScore(fetchAvailableDevices())) {}

Tracer::Tracer(const Device& device) { m_deviceTracer = createDeviceTracer(device.type(), device.m_indexWithinDeviceType); }

Rays Tracer::trace(const Beamline& beamline, const TraceOptions& options) {
    auto rays = m_deviceTracer->trace(detail::toModel(beamline), options);
    assert(rays.isValid() && "one or more recorded attributes have different number of items. this should never happen, please report to the developers.");
    return rays;  // rvo
}

}  // namespace rayx
