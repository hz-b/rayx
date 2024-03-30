#pragma once

#include <cstring>

#include "Core.h"
#include "Shader/InvocationState.h"

namespace RAYX {

class Beamline;

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

// TODO(Sven): make nice api for trace result
struct TraceResult {
    RayHistory events;
    std::vector<int> offsets;
    std::vector<int> sizes;
    int count;
};

/**
 * @brief DeviceTracer is an interface to a tracer implementation
 * we use this interface to remove the actual implementation from the rayx api
 */
class RAYX_API DeviceTracer {
  public:
    virtual ~DeviceTracer() = default;

    virtual BundleHistory trace(
        const Beamline&,
        Sequential sequential,
        uint64_t max_batch_size,
        int THREAD_COUNT = 1,
        unsigned int maxEvents = 1,
        int startEventID = 0
    ) = 0;

    inline void setPushConstants(const PushConstants* p) {
        std::memcpy(&m_pushConstants, p, sizeof(PushConstants));
    }

  protected:
    PushConstants m_pushConstants;
};

}  // namespace RAYX
