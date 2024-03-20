#pragma once

#ifndef NO_GPU_TRACER

#include "Core.h"
#include "Tracer.h"
#include "Shader/InvocationState.h"

namespace RAYX {

class RAYX_API GpuTracer : public Tracer {
  public:
    GpuTracer();
    ~GpuTracer();

    std::vector<Ray> traceRaw(const TraceRawConfig&) override { return {}; }
    void setPushConstants(const PushConstants*) {};
};

}  // namespace RAYX

#endif // NO_GPU_TRACER
