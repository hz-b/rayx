#pragma once

#include "Core.h"
#include "Tracer/RayList.h"
#include "Tracer/Tracer.h"

namespace RAYX {

class RAYX_API CpuTracer : public Tracer {
  public:
    CpuTracer();
    ~CpuTracer();

    RayList trace(const Beamline&) override;
};

}  // namespace RAYX
