#pragma once

#include "Core.h"
#include "Tracer/RayList.h"
#include "Tracer/Tracer.h"

namespace RAYX {
/**
 * @brief The CPU tracer can replace the Vulkan tracer to run all shader compute
 * locally on CPU and RAM. |CPU| --> |SHADER(OnCPU)|--> |CPU|
 *
 */
class RAYX_API CpuTracer : public Tracer {
  public:
    /**
     * @brief Constructs a new *CPU* Tracer object that utlizes the compute
     * shader code.
     *
     */
    CpuTracer();
    ~CpuTracer();

    RayList trace(const Beamline&) override;
};

}  // namespace RAYX
