#include "Tracer.h"

#include <algorithm>

#include "Platform.h"
#include "SimpleTracer.h"

namespace {

using Dim = alpaka::DimInt<1>;
using Idx = int32_t;
using GpuAcc = RAYX::DefaultGpuAcc<Dim, Idx>;
using CpuAcc = RAYX::DefaultCpuAcc<Dim, Idx>;

inline int64_t getDeviceCountForPlatform(RAYX::Tracer::Platform platform) {
    switch (platform) {
    case RAYX::Tracer::Platform::Gpu:
#ifdef GPU_TRACER
        return alpaka::getDevCount(alpaka::Platform<GpuAcc>());
#else
        return 0;
#endif

    default: // case RAYX::Tracer::Platform::Cpu
        return alpaka::getDevCount(alpaka::Platform<CpuAcc>());
    }
}

inline std::string getDeviceName(RAYX::Tracer::Platform platform, int deviceIndex) {
    switch (platform) {
        case RAYX::Tracer::Platform::Gpu: {
#ifdef GPU_TRACER
            auto dev = RAYX::getDevice<GpuAcc>(deviceIndex);
            return alpaka::getName(dev);
#else
            assert(false && "Gpu support was disabled during build. Cannot get device name");
            return "device-not-found";
#endif

        }
        default: { // case Tracer::Platform::Cpu
            auto dev = RAYX::getDevice<CpuAcc>(deviceIndex);
            return alpaka::getName(dev);
        }
    }
}

inline std::shared_ptr<RAYX::DeviceTracer> createDeviceTracer(RAYX::Tracer::Platform platform, int deviceIndex) {
    switch (platform) {
    case RAYX::Tracer::Platform::Gpu:
#ifdef GPU_TRACER
            return std::make_shared<RAYX::SimpleTracer<GpuAcc>>(deviceIndex);
#else
            RAYX_WARN
                << "Gpu Tracer was disabled during build."
                << " Falling back to Cpu Tracer."
                << " Add '-x' flag on launch to use the Cpu Tracer directly"
            ;
            [[fallthrough]];
#endif
    default: // case RAYX::Tracer::Platform::Cpu
        return std::make_shared<RAYX::SimpleTracer<CpuAcc>>(deviceIndex);
    }
}

} // unnamed namespace

namespace RAYX {

Tracer::Tracer(Platform platform, int deviceIndex) :
    m_platform(platform),
    m_deviceIndex(deviceIndex)
{
    m_deviceTracer = createDeviceTracer(platform, deviceIndex);
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

int64_t Tracer::deviceCount(Platform platform) {
    return getDeviceCountForPlatform(platform);
}

std::string Tracer::deviceName(Platform platform, int deviceIndex) {
    return getDeviceName(platform, deviceIndex);
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
