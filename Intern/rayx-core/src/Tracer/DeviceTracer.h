#pragma once

#include <cstring>
#include <vector>

#include "Core.h"
#include "Rays.h"
#include "Shader/InvocationState.h"

namespace RAYX {

/**
 * @brief DeviceTracer is an interface to a tracer implementation
 * we need this interface to remove the actual implementation from the rayx api
 */
class RAYX_API DeviceTracer {
  public:
    virtual ~DeviceTracer() = default;

    virtual Rays trace(const Group& beamline, Sequential sequential, const ObjectRecordMask& objectRecordMask, const RayAttrFlag attr,
                       const int maxEvents, const int maxBatchSize) = 0;
};

}  // namespace RAYX
