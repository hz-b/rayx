#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Ray.h"
#include "Shared/Constants.h"

// if no `--batch` option is given, this it the batch size.
const uint64_t DEFAULT_BATCH_SIZE = 100000;

// Abstract Tracer base class.
namespace RAYX {

struct TraceRawConfig {
    std::vector<Ray> m_rays;
    double m_rayIdStart;
    double m_numRays;
    double m_randomSeed;
    double m_maxSnapshots;
    MaterialTables m_materialTables;
    std::vector<Element> m_elements;
};

/// A 'snapshot' of a ray, at the time where it undergoes some event.
/// The event type is specified by `m_weight`.
using Event = Ray;

/// Contains all the snapshots of a single Ray.
using Snapshots = std::vector<Event>;

/// Contains the snapshots of all the rays
/// Given a `Rays rays;`
/// rays[i][j] is the j'th snapshot of the i'th ray.
using Rays = std::vector<Snapshots>;

class RAYX_API Tracer {
  public:
    Tracer() {}
    virtual ~Tracer() {}

    // This will call traceRaw.
    // Everything happening in each traceRaw implementation should be extracted to this function instead.
    // See `Rays` for information about the return value.
    Rays trace(const Beamline&, uint64_t max_batch_size);

    // Useful for GPU Tracing
    struct PushConstants {  // TODO(Jannis): PushConstants is not an expressive name. Rename to something like TracerConfig
        double rayIdStart;
        double numRays;
        double randomSeed;
        double maxSnapshots;
    };

  protected:
    // where the actual tracing happens.
    // std::vector<Ray> will contain all snapshots for all Rays (and also the W_UNINIT rays).
    virtual std::vector<Ray> traceRaw(const TraceRawConfig&) = 0;
    virtual void setPushConstants(const PushConstants*) = 0;
};

// TODO deprecate these functions and all of their uses.
RAYX_API std::vector<Ray> extractLastSnapshot(const Rays& rays);
std::vector<Ray> extracFirstSnapshot(const Rays& rays);
std::vector<Ray> extracNthSnapshot(const Rays& rays, int snapshotID);
RAYX_API Rays convertToRays(const std::vector<Ray>& rays);

}  // namespace RAYX
