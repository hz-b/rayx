#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Shader/Constants.h"
#include "Shader/InvocationState.h"
#include "Shader/Ray.h"

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
    double m_startEventID;
    MaterialTables m_materialTables;
    std::vector<Element> m_elements;
};

/// Contains all the events of a single Ray in chronological order.
using RayHistory = std::vector<Ray>;

/// Contains all events for some bundle of rays.
/// Given a `BundleHistory hist;`
/// hist[i][j] is the j'th event of the i'th ray of the bundle.
using BundleHistory = std::vector<RayHistory>;

class RAYX_API Tracer {
  public:
    virtual ~Tracer() {}

    // This will call the trace implementation of a subclass
    // See `BundleHistory` for information about the return value.
    // `max_batch_size` corresponds to the maximal number of rays that will be put into `traceRaw` in one batch.
    virtual BundleHistory trace(
        const Beamline&,
        Sequential sequential,
        uint64_t max_batch_size,
        int THREAD_COUNT = 1,
        unsigned int maxEvents = 1,
        int startEventID = 0
    ) = 0;

    void setDevice(int deviceID);

  protected:
    // TODO Why are the PushConstants not part of the TraceRawConfig?
    // TODO The TraceRawConfig is supposed to contain all information relevant for tracing.
    virtual void setPushConstants(const PushConstants*) = 0;

    // -1 means no device is selected.
    int m_deviceID = -1;
};

// TODO deprecate these functions and all of their uses.
RAYX_API std::vector<Ray> extractLastEvents(const BundleHistory& hist);
RAYX_API BundleHistory convertToBundleHistory(const std::vector<Ray>& rays);

}  // namespace RAYX
