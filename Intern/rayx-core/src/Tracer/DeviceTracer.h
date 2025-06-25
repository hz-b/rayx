#pragma once

#include <cstring>
#include <vector>

#include "Core.h"
#include "RaySoA.h"
#include "Shader/InvocationState.h"

namespace RAYX {

/**
 * @brief DeviceTracer is an interface to a tracer implementation
 * we use this interface to remove the actual implementation from the rayx api
 */
class RAYX_API DeviceTracer {
  public:
    virtual ~DeviceTracer() = default;

    virtual RaySoA trace(const Group&, Sequential sequential, const int maxBatchSize, const int maxEvents, std::shared_ptr<bool[]> recordMask,
                         const RayAttrFlag attr) = 0;
};

}  // namespace RAYX
