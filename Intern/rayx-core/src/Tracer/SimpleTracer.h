#pragma once

#include <functional>
#include <memory>

#include "Core.h"
#include "Tracer.h"
#include "Shader/InvocationState.h"

namespace RAYX {

/**
 * @brief SimpleTracer sequentially executes tracing in batches the CPU or GPU
 */
class RAYX_API SimpleTracer : public Tracer {
  public:
    enum class Platform {
        Cpu,
        Gpu,
    };

    struct TraceResult {
        RayHistory events;
        std::vector<int> offsets;
        std::vector<int> sizes;
        int count;
    };

    /**
     * @brief Constructs SimpleTracer for the desired platform
     * @param platform provide the desired platform
     */
    SimpleTracer(Platform platform);
    ~SimpleTracer();

    BundleHistory trace(
        const Beamline&,
        Sequential sequential,
        uint64_t max_batch_size,
        int THREAD_COUNT = 1,
        unsigned int maxEvents = 1,
        int startEventID = 0
    ) override;

    void setPushConstants(const PushConstants*) override;

  protected:
    PushConstants m_pushConstants;

    using TraceBatchOnPlatformFn = TraceResult(const TraceRawConfig&, const PushConstants&);
    std::function<TraceBatchOnPlatformFn> m_traceBatchOnPlatformFn;
};

}  // namespace RAYX
