#pragma once

#include <alpaka/alpaka.hpp>

#include "Debug/Debug.h"

namespace RAYX {

template <typename Acc>
constexpr inline auto getBlockSize() {
    using Idx = alpaka::Idx<Acc>;
    using Dim = alpaka::Dim<Acc>;

    auto impl = []() -> Idx {
#if defined(ALPAKA_ACC_GPU_CUDA_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccGpuCudaRt<Dim, Idx>>) return 128;
#endif

#if defined(ALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuOmp2Blocks<Dim, Idx>>) return 1;
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuSerial<Dim, Idx>>) return 1;
#endif

        return 0;  // unimplemented
    };

    constexpr Idx blockSize = impl();
    static_assert(blockSize != static_cast<Idx>(0));  // getBlockSize() is not implemented for Acc

    return blockSize;
}

template <typename Acc>
inline auto getWorkDivForAcc(alpaka::Idx<Acc> numElements) {
    using Idx = alpaka::Idx<Acc>;
    using Dim = alpaka::Dim<Acc>;

    constexpr int blockSize = getBlockSize<Acc>();
    const int gridSize = (numElements - 1) / blockSize + 1;

    using Vec = alpaka::Vec<Dim, Idx>;
    return alpaka::WorkDivMembers<Dim, Idx>{
        Vec{gridSize},
        Vec{blockSize},
        Vec{1},
    };
}

}  // namespace RAYX
