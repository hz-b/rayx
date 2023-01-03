#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Core.h"
#include "Model/Beamline/Beamline.h"
#include "Ray.h"

// Abstract Tracer base class.
namespace RAYX {

struct TraceRawConfig {
    std::vector<Ray> m_rays;
    double m_rayIdStart;
    double m_numRays;
    double m_randomSeed;
    double m_maxSnapshots;
    MaterialTables m_materialTables;
    const std::vector<std::shared_ptr<OpticalElement>>& m_OpticalElements;
};

class RAYX_API Tracer {
  public:
    Tracer() {}
    virtual ~Tracer() {}

    // This will call traceRaw.
    // Everything happening in each traceRaw implementation should be extracted to this function instead.
    // std::vector<Ray> will only contain the last snapshot for each ray.
    std::vector<Ray> trace(const Beamline&);

  protected:
    // where the actual tracing happens.
    // std::vector<Ray> will contain all snapshots for all Rays (and also the W_UNINIT rays).
    virtual std::vector<Ray> traceRaw(const TraceRawConfig&) = 0;
};

}  // namespace RAYX
