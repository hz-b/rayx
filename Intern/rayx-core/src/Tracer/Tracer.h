#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Core.h"
#include "DeviceConfig.h"
#include "DeviceTracer.h"
#include "Rays.h"

// Abstract Tracer base class.
namespace RAYX {

// this value is picked in a 'good' way if it can divide number of rays without rest. for a number of rays picked by humans, this
// value is probably good. though, if it could be power of two, the shader would benefit
constexpr int DEFAULT_BATCH_SIZE = 100000;

constexpr int defaultMaxEvents(const int numObjects) { return numObjects * 2 + 8; }

class RAYX_API Tracer {
  public:
    /**
     * @brief Construct a new Tracer object
     * @param deviceConfig Configuration for the device to be used for tracing
     */
    Tracer(const DeviceConfig& deviceConfig = DeviceConfig().enableBestDevice());

    // This will call the trace implementation of a subclass
    // See `BundleHistory` for information about the return value.
    // `max_batch_size` corresponds to the maximal number of rays that will be put into `traceRaw` in one batch.

    /**
     *  @brief Trace rays through the given group
     *  @param group The group to trace rays through
     *  @param sequential Whether to trace rays sequentially or non-sequentially
     *  @param objectRecordMask Object record mask specifying which sources and elements to record
     *  @param attrRecordMask Attributes to record for each ray
     *  @param maxEvents Optional maximum number of events to trace per ray (only used in non-sequential tracing)
     *  @param maxBatchSize Optional maximum batch size for tracing
     *  @return A `Rays` struct containing the traced ray attributes, specified by `attrRecordMask` and filtered by `objectRecordMask`
     */
    Rays trace(const Group& group, const Sequential sequential = Sequential::No, const ObjectMask& objectRecordMask = ObjectMask::all(),
               const RayAttrMask attrRecordMask = RayAttrMask::All, std::optional<int> maxEvents = std::nullopt,
               std::optional<int> maxBatchSize = std::nullopt);

  private:
    std::shared_ptr<DeviceTracer> m_deviceTracer;
};

}  // namespace RAYX
