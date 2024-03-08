#pragma once

#ifndef NO_GPU_TRACER

#include "Core.h"
#include "Tracer.h"
#include "Shader/InvocationState.h"

namespace RAYX {

/**
 * @brief The CPU tracer can replace the Vulkan tracer to run all shader compute
 * locally on CPU and RAM. |CPU| --> |SHADER(OnCPU)|--> |CPU|
 *
 */
class RAYX_API GpuTracer : public Tracer {
public:
    /**
     * @brief Constructs a new *CPU* Tracer object that utlizes the compute
     * shader code.
     *
     */
    GpuTracer();
    ~GpuTracer();

    std::vector<Ray> traceRaw(const TraceRawConfig&) override;
    void setPushConstants(const PushConstants*) override;

private:
    PushConstants m_pushConstants;
};

}  // namespace RAYX

#endif // NO_GPU_TRACER
