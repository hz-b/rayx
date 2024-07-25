#pragma once

#include <cstring>
#include <future>

#include "Core.h"
#include "Material/Material.h"
#include "Shader/InvocationState.h"

namespace RAYX {

class Beamline;

/// Expresses whether we force sequential tracing, or we use dynamic tracing.
/// We prefer this over a boolean, as calling eg. the trace function with an argument of `true` has no obvious meaning.
/// On the other hand calling it with `Sequential::Yes` makes the meaning more clear.
enum class Sequential { No, Yes };

/// Contains all the events of a single Ray in chronological order.
using RayHistory = std::vector<Ray>;

/// Contains all events for some bundle of rays.
/// Given a `BundleHistory hist;`
/// hist[i][j] is the j'th event of the i'th ray of the bundle.
using BundleHistory = std::vector<RayHistory>;

/**
 * @brief DeviceTracer is an interface to a tracer implementation
 * we use this interface to remove the actual implementation from the rayx api
 */
class RAYX_API DeviceTracer {
  public:
    virtual ~DeviceTracer() = default;

    // virtual BundleHistory trace(const Beamline&, Sequential sequential, uint64_t max_batch_size, int THREAD_COUNT = 1, unsigned int maxEvents = 1,
    // int startEventID = 0) = 0;

    struct BeamlineInput {
        std::vector<Element> elements;
        std::vector<Ray> rays;
        MaterialTables materialTables;
        double randomSeed;
    };
    using BeamlineInputPtr = std::shared_ptr<BeamlineInput>;

    struct BatchInput {
        int raysOffset;
        int raysSize;
    };

    struct RAYX_API BatchOutput {
        std::vector<int> eventCounts;
        std::vector<int> eventOffsets;
        std::vector<Ray> events;
    };

    virtual BatchOutput traceBatch(const BeamlineInputPtr& beamlineInput, const BatchInput& batchInput) = 0;

  protected:
    PushConstants m_pushConstants;
};

}  // namespace RAYX

// TODO: where RAYX_API ???
