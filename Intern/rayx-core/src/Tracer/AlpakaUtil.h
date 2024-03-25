#pragma once

// -------- Accelerator --------

struct AccNull {};

template <typename Dim, typename Idx>
#if defined(ALPAKA_ACC_GPU_CUDA_ENABLED) || defined(ALPAKA_ACC_GPU_HIP_ENABLED)
using DefaultGpuAcc = alpaka::AccGpuCudaRt<Dim, Idx>;
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
    return alpaka::isAccelerator<Acc>;
}

// -------- Accelerator Specifics --------

template <typename Acc>
constexpr inline auto getBlockSize() {
    using Idx = alpaka::Idx<Acc>;
    using Dim = alpaka::Dim<Acc>;

    auto impl = [] () -> Idx {
#if defined(ALPAKA_ACC_GPU_CUDA_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccGpuCudaRt<Dim, Idx>>) {
            return 128;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_THREADS_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuThreads<Dim, Idx>>) {
            return 1;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuOmp2Blocks<Dim, Idx>>) {
            return 1;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_OMP2_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuOmp2Threads<Dim, Idx>>) {
            return 1;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuSerial<Dim, Idx>>) {
            return 1;
        }
#endif

        return 0;
    };

    constexpr Idx blockSize = impl();
    static_assert(blockSize != static_cast<Idx>(0));

    return blockSize;
}

template <typename Acc>
inline auto getWorkDivForAcc(alpaka::Idx<Acc> numElements) {
    using Idx = alpaka::Idx<Acc>;
    using Dim = alpaka::Dim<Acc>;

    constexpr int blockSize = getBlockSize<Acc>();
    const int gridSize = (numElements - 1) / blockSize + 1;

    using Vec = alpaka::Vec<Dim, Idx>;
    return alpaka::WorkDivMembers<Dim, Idx> {
        Vec{gridSize},
        Vec{blockSize},
        Vec{1},
    };
}

// -------- Buffers and Views --------

template <typename Acc>
inline auto pickFirstDevice() {
    const auto platform = alpaka::Platform<Acc>();
    const auto dev = alpaka::getDevByIdx(platform, 0);
    RAYX_VERB
        << "found " << alpaka::getDevCount(platform) << ""
        << "device(s) for platform '" << alpaka::getAccName<Acc>() << "'."
        << "picking '" << alpaka::getName(dev) << "'"
    ;
    return std::make_tuple(platform, dev);
};

template <typename Idx, typename T, typename Queue, typename Extent>
inline auto createBuffer(Queue& queue, Extent size) {
    return alpaka::allocAsyncBufIfSupported<T, Idx>(queue, size);
}

template <typename Idx, typename Extent, typename T, typename Queue, typename Cpu>
inline auto createBuffer(Queue& queue, const std::vector<T>& data, const Cpu& cpu) {
    auto dataView = alpaka::createView(cpu, data);
    auto buf = alpaka::allocAsyncBufIfSupported<T, Idx>(queue, Extent{data.size()});
    alpaka::memcpy(queue, buf, dataView, Extent{data.size()});
    return buf;
}

template <typename TBuf>
inline auto bufToSpan(TBuf& buf) {
    return std::span(alpaka::getPtrNative(buf), alpaka::getExtents(buf)[0]);
}
