#pragma once

#include <cstring>
#include <vector>

#include "Core.h"
#include "RaySoA.h"
#include "Shader/InvocationState.h"

namespace RAYX {

// Small helper to make passing the full record mask easier.
inline std::vector<bool> fullRecordMask(size_t numElements) { return std::vector<bool>(numElements, true); }

/**
 * @brief DeviceTracer is an interface to a tracer implementation
 * we use this interface to remove the actual implementation from the rayx api
 */
class RAYX_API DeviceTracer {
  public:
    virtual ~DeviceTracer() = default;

    virtual RaySoA trace(const Group& beamline, Sequential sequential, const int maxBatchSize, const int maxEvents,
                         const std::vector<bool>& recordMask, const RayAttrFlag attr) = 0;
};

}  // namespace RAYX
