#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shared/Constants.h"
#include "Shared/Ray.h"

// if no `--batch` option is given, this it the batch size.
const uint64_t DEFAULT_BATCH_SIZE = 100000;

// Abstract Tracer base class.
namespace RAYX {

/// Expresses whether we force sequential tracing, or we use dynamic tracing.
/// We prefer this over a boolean, as calling eg. the trace function with an argument of `true` has no obvious meaning.
/// On the other hand calling it with `Sequential::Yes` makes the meaning more clear.
enum class Sequential { No, Yes };

struct TraceRawConfig {
    std::vector<Ray> m_rays;
    double m_rayIdStart;
    double m_numRays;
    double m_randomSeed;
    double m_maxEvents;
    MaterialTables m_materialTables;
    std::vector<Element> m_elements;
};

// Useful for GPU Tracing
struct PushConstants {  // TODO(Jannis): PushConstants is not an expressive name. Rename to something like TracerConfig
    double rayIdStart;
    double numRays;
    double randomSeed;
    double maxEvents;
    double sequential;
};

/// A 'snapshot' of a ray, at the time where it undergoes some event.
/// The event type is specified by `m_eventType`.
// TODO: split Event and Ray into separate types, because `Event` needs an m_eventType (aka m_weight), whereas Ray does not.
using Event = Ray;

/// Contains all the events of a single Ray in chronological order.
using RayHistory = std::vector<Event>;

/// Contains all events for some bundle of rays.
/// Given a `BundleHistory hist;`
/// hist[i][j] is the j'th event of the i'th ray of the bundle.
using BundleHistory = std::vector<RayHistory>;

class RAYX_API Tracer {
  public:
    Tracer() {}
    virtual ~Tracer() {}

    // This will call traceRaw.
    // Everything happening in each traceRaw implementation should be extracted to this function instead.
    // See `BundleHistory` for information about the return value.
    BundleHistory trace(const Beamline&, uint64_t max_batch_size, int THREAD_COUNT = 1);

    void setDevice(int deviceID);

  protected:
    // where the actual tracing happens.
    // std::vector<Ray> will contain all events for all Rays (and also the ETYPE_UNINIT events).
    virtual std::vector<Ray> traceRaw(const TraceRawConfig&) = 0;
    virtual void setPushConstants(const PushConstants*) = 0;
    // -1 means no device is selected.
    int m_deviceID = -1;
};

// TODO deprecate these functions and all of their uses.
RAYX_API std::vector<Ray> extractLastEvents(const BundleHistory& hist);
RAYX_API BundleHistory convertToBundleHistory(const std::vector<Ray>& rays);

}  // namespace RAYX
