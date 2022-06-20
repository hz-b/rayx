#pragma once

#include "Core.h"
#include "Tracer/RayList.h"
#include "Tracer/Tracer.h"

struct Element {
    glm::dmat4 surfaceParams;
    glm::dmat4 inTrans;
    glm::dmat4 outTrans;
    glm::dmat4 objectParameters;
    glm::dmat4 elementParameters;
};

namespace RAYX {

class RAYX_API CpuTracer : public Tracer {
  public:
    CpuTracer();
    ~CpuTracer();

    RayList trace(const Beamline&) override;
};

}  // namespace RAYX
