#pragma once

#include <functional>

#include "Core.h"
#include "Tracer.h"
#include "Shader/InvocationState.h"

namespace RAYX {

/**
 * @brief The CPU tracer can replace the Vulkan tracer to run all shader compute
 * locally on CPU and RAM. |CPU| --> |SHADER(OnCPU)|--> |CPU|
 *
 */
class RAYX_API SimpleTracer : public Tracer {
  public:
    enum class Platform {
        Cpu,
        Gpu,
    };

    /**
     * @brief Constructs a new *CPU* Tracer object that utlizes the compute
     * shader code.
     *
     */
    SimpleTracer(Platform platform);
    ~SimpleTracer();

    std::vector<Ray> traceRaw(const TraceRawConfig&) override;

    void setPushConstants(const PushConstants*) override;

protected:
    PushConstants m_pushConstants;

    using TraceFn = std::vector<Ray>(const TraceRawConfig&, const PushConstants&);
    std::function<TraceFn> m_traceFn;
};

}  // namespace RAYX
