#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "ObjectMask.h"
#include "Rays.h"
#include "rayx/Core.h"
#include "DeviceTracer.h"

namespace rayx {

struct Device;
struct Beamline;

/** @brief Trace mode for ray tracing.
 * In Sequential tracing a ray can only hit each object in order and at most once.
 */
struct SequentialTraceModeType {};
inline constexpr SequentialTraceModeType SequentialTraceMode;

/** @brief Trace mode for ray tracing.
 * In Non-Sequential tracing a ray can hit objects in any order and can hit the same object multiple times.
 * The maxEventsPerPath parameter can be used to limit the number of events traced per ray path, which can help control memory usage and performance.
 * If not specified, a default maximum number of events will be used, which is based on the number of objects in the beamline and is intended to be
 * sufficient for most cases.
 */
struct NonSequentialTraceMode {
    std::optional<int> maxEventsPerPath = std::nullopt;
};

/** @brief Trace mode for ray tracing, which can be either sequential or non-sequential. */
using TraceMode = std::variant<SequentialTraceModeType, NonSequentialTraceMode>;

/**
 * @brief Options for ray tracing
 * traceMode: Whether to trace rays sequentially or non-sequentially.
 * objectRecordMask: Object record mask specifying which sources and elements to record. Only the events involving the recorded objects will be
 * included in the output. attrRecordMask: Attributes to record for each ray. Only the recorded attributes will be included in the output. batchSize:
 * Optional maximum batch size for tracing. If not specified, a default batch size will be used. This parameter can be useful for controlling memory
 * usage and performance.
 */
struct TraceOptions {
    TraceMode traceMode          = NonSequentialTraceMode{};
    ObjectMask objectRecordMask  = ObjectMaskAll;
    RayAttrMask attrRecordMask   = RayAttrMask::All;
    std::optional<int> batchSize = std::nullopt;
};

class RAYX_API Tracer {
  public:
    /**
     * @brief Construct a new Tracer object
     * @note The best available device will be automatically selected based on the highest score. If no devices are available, an exception will be
     * thrown.
     */
    Tracer();

    /**
     * @brief Construct a new Tracer object
     * @param device The device to use for ray tracing.
     */
    Tracer(const Device& device);

    /**
     *  @brief Trace rays through the given group
     *  @param beamline The beamline to trace rays through.
     *  @param options Options for ray tracing, including trace mode, object record mask, attribute record mask, and batch size. If not specified,
     * default options will be used.
     *  @return A `Rays` struct containing the traced ray attributes, specified by `attrRecordMask` and filtered by `objectRecordMask`
     */
    Rays trace(const Beamline& beamline, const TraceOptions& options = {});

  private:
    std::shared_ptr<detail::DeviceTracer> m_deviceTracer;
};

}  // namespace rayx
