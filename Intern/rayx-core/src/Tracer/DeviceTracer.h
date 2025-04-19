#pragma once

#include <cstring>
#include <vector>

#include "Core.h"
#include "Shader/InvocationState.h"

namespace RAYX {

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

    virtual BundleHistory trace(const Group&, Sequential sequential, const int maxBatchSize, const int maxEvents) = 0;
};

}  // namespace RAYX
