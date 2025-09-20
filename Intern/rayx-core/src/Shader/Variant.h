#pragma once

#if defined(RAYX_CUDA_ENABLED)
#include <cuda/std/variant>
#else
#include <variant>
#endif

namespace RAYX {

#if defined(RAYX_CUDA_ENABLED)
namespace variant = cuda::std;
#else
namespace variant = std;
#endif

}  // namespace RAYX
