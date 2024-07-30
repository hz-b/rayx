#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Core.h"
#include "DeviceConfig.h"
#include "DeviceTracer.h"
#include "Shader/Ray.h"

// Abstract Tracer base class.
namespace RAYX {

// if no `--batch` option is given, this it the batch size.
const uint64_t DEFAULT_BATCH_SIZE = 100000;

class RAYX_API Tracer {
  public:
    /**
     * @brief Constructs Tracer for the desired platform
     * @param platform specify the platform
     * @param deviceIndex index of the picked divice on specified platform
     */
    Tracer(const DeviceConfig& deviceConfig);

    // This will call the trace implementation of a subclass
    // See `BundleHistory` for information about the return value.
    // `max_batch_size` corresponds to the maximal number of rays that will be put into `traceRaw` in one batch.
    BundleHistory trace(const Beamline&, Sequential sequential, uint64_t max_batch_size, int THREAD_COUNT = 1, uint32_t maxEvents = 1,
                        int startEventID = 0);

    static int defaultMaxEvents(const Beamline* beamline = nullptr);

  private:
    std::shared_ptr<DeviceTracer> m_deviceTracer;
};

// TODO deprecate these functions and all of their uses.
RAYX_API std::vector<Ray> extractLastEvents(const BundleHistory& hist);
RAYX_API BundleHistory convertToBundleHistory(const std::vector<Ray>& rays);

}  // namespace RAYX
