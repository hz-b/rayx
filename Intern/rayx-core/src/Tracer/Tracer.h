#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DeviceTracer.h"
#include "Core.h"
#include "Shader/Ray.h"

// if no `--batch` option is given, this it the batch size.
const uint64_t DEFAULT_BATCH_SIZE = 100000;

// Abstract Tracer base class.
namespace RAYX {

class RAYX_API Tracer {
  public:
    enum class Platform {
        Cpu,
        Gpu,
    };

    /**
     * @brief Constructs Tracer for the desired platform
     * @param platform specify the platform
     * @param deviceIndex index of the picked divice on specified platform
     */
    Tracer(Platform platform, int deviceIndex = 0);

    // This will call the trace implementation of a subclass
    // See `BundleHistory` for information about the return value.
    // `max_batch_size` corresponds to the maximal number of rays that will be put into `traceRaw` in one batch.
    BundleHistory trace(
        const Beamline&,
        Sequential sequential,
        uint64_t max_batch_size,
        int THREAD_COUNT = 1,
        unsigned int maxEvents = 1,
        int startEventID = 0
    );

    inline Platform platform() { return m_platform; }
    inline int deviceIndex() { return m_deviceIndex; }

    static int deviceCount(Platform platform);
    static std::string deviceName(Platform platform, int deviceIndex);

  private:
    // TODO Why are the PushConstants not part of the TraceRawConfig?
    // TODO The TraceRawConfig is supposed to contain all information relevant for tracing.
    void setPushConstants(const PushConstants*);

    const Platform m_platform;
    const int m_deviceIndex;

    std::shared_ptr<DeviceTracer> m_deviceTracer;
};

// TODO deprecate these functions and all of their uses.
RAYX_API std::vector<Ray> extractLastEvents(const BundleHistory& hist);
RAYX_API BundleHistory convertToBundleHistory(const std::vector<Ray>& rays);

}  // namespace RAYX
