#pragma once

namespace RAYX {
// TODO(OS): struct has to be a series of objects (vector) for multi-pipeline support
// TODO(OS): Move this to ´Shared/´
struct ComputeParams {
    double rayIdStart;
    double numRays;
    double randomSeed;
    double maxSnapshots;  // FIXME(OS) : Only used by CPU
    int i_bounce = 0;
};
// Vulkan usage
using PushConstants = ComputeParams;
}  // namespace RAYX