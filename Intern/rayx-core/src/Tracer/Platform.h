#pragma once

#include <alpaka/alpaka.hpp>

namespace RAYX {

// declare invalid Acc
struct AccNull {};

template <typename Dim, typename Idx>
#if defined(ALPAKA_ACC_GPU_CUDA_ENABLED)
#define GPU_TRACER
using DefaultGpuAcc = alpaka::AccGpuCudaRt<Dim, Idx>;
#elif defined(ALPAKA_ACC_GPU_HIP_ENABLED)
using DefaultGpuAcc = alpaka::AccGpuHipRt<Dim, Idx>;
#else
using DefaultGpuAcc = AccNull;
#endif

template <typename Dim, typename Idx>
#if defined(ALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED)
using DefaultCpuAcc = alpaka::AccCpuOmp2Blocks<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_SEQ_T_OMP2_ENABLED)
using DefaultCpuAcc = alpaka::AccCpuOmp2Threads<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED)
using DefaultCpuAcc = alpaka::AccCpuTbbBlocks<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_SEQ_T_THREADS_ENABLED)
using DefaultCpuAcc = alpaka::AccCpuThreads<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED)
using DefaultCpuAcc = alpaka::AccCpuSerial<Dim, Idx>;
#else
using DefaultCpuAcc = AccNull;
#endif

template <typename Dim, typename Idx>
using DefaultAcc = std::conditional_t<
    alpaka::isAccelerator<DefaultGpuAcc<Dim, Idx>>,
    DefaultGpuAcc<Dim, Idx>,
    DefaultCpuAcc<Dim, Idx>
>;

template <typename Acc>
constexpr bool isAccAvailable() {
    return !std::is_same_v<Acc, AccNull> && alpaka::isAccelerator<Acc>;
}

// test if we have a minimum required Accelerator
static_assert(isAccAvailable<DefaultCpuAcc<alpaka::DimInt<1>, int>>()); // test if at least one Cpu Acc is available.

} // namespace RAYX
