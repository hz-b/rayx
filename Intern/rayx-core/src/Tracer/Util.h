#pragma once

#include <alpaka/alpaka.hpp>
#include "Debug/Instrumentor.h"
#include <vector>
#include <optional>

namespace RAYX {

inline int ceilIntDivision(const int dividend, const int divisor) { return (divisor + dividend - 1) / divisor; }

inline int nextPowerOfTwo(const int value) { return static_cast<int>(glm::pow(2, glm::ceil(glm::log(value) / glm::log(2)))); }

/// conditionally allocate buffer with specified minimum size.
/// if the buffer already fulfills size requirements, this function does nothing. thus, this function never shrinks a buffer.
/// actual allocation size is nextPowerOfTwo(size).
/// this function is designed to optimize the repetitive use of the buffer with potentially different size requirements (e.g. tracing multiple
/// beamlines one after the other)
template <typename Queue, typename Buf>
inline void allocBuf(Queue q, std::optional<Buf>& buf, const int size) {
    using Idx = alpaka::Idx<Buf>;
    using Elem = alpaka::Elem<Buf>;

    const auto shouldAlloc = !buf || alpaka::getExtents(*buf)[0] < size;
    if (shouldAlloc) RAYX_VERB << (!buf ? "new alloc on device: " : "realloc on device: ") << nextPowerOfTwo(size * sizeof(Elem)) << " bytes";
    if (shouldAlloc) buf = alpaka::allocAsyncBufIfSupported<Elem, Idx>(q, nextPowerOfTwo(size));
}

inline bool checkTooManyEvents(const std::vector<Ray>& compactEvents, const std::vector<int>& compactEventCounts,
                               const std::vector<int>& compactEventOffsets, const int batchSize) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (int i = 0; i < batchSize; ++i) {
        if (0 < compactEventCounts[i]) {
            const auto offset = compactEventOffsets[i];
            const auto count = compactEventCounts[i];
            const auto lastEventIndex = offset + count - 1;
            if (compactEvents[lastEventIndex].m_eventType == EventType::TooManyEvents) return true;
        }
    }

    return false;
}

inline void collectCompactEventsIntoBundleHistory(BundleHistory& bundleHistory, const std::vector<Ray>& compactEvents,
                                                  const std::vector<int>& compactEventCounts, const std::vector<int>& compactEventOffsets,
                                                  const int batchSize) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (int i = 0; i < batchSize; i++) {
        const auto begin = compactEvents.data() + compactEventOffsets[i];
        const auto end = begin + compactEventCounts[i];

        // add events to history, only if there are events
        bool hasEvents = 0 < std::distance(begin, end);
        if (hasEvents) bundleHistory.emplace_back(begin, end);
    }
}

// TODO: maybe make a PR to alpaka for alpaka::Acc<Dev> to extract Acc from Dev<Platform<Acc>>
template <typename Acc, typename DevAcc, typename Queue, typename Kernel, typename... Args>
inline void execWithValidWorkDiv(DevAcc devAcc, Queue q, const int numElements, const int maxBlockSize, const Kernel& kernel, Args&&... args) {
    const auto conf = alpaka::KernelCfg<Acc>{
        .gridElemExtent = numElements,
        .threadElemExtent = 1,
        .blockThreadMustDivideGridThreadExtent = false,
    };

    auto workDiv = alpaka::getValidWorkDiv(conf, devAcc, kernel, std::forward<Args>(args)...);
    if (maxBlockSize < workDiv.m_blockThreadExtent[0]) {
        workDiv.m_blockThreadExtent = maxBlockSize;
        workDiv.m_gridBlockExtent = ceilIntDivision(numElements, maxBlockSize);
    }

    RAYX_VERB << "executing kernel with launch config: "
              << "blocks = " << workDiv.m_gridBlockExtent[0] << ", "
              << "threads = " << workDiv.m_blockThreadExtent[0] << ", "
              << "elements = " << workDiv.m_threadElemExtent[0];

    alpaka::exec<Acc>(q, workDiv, kernel, std::forward<Args>(args)...);
}

struct GatherIndicesKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& __restrict acc, int* __restrict srcIndices, const int* __restrict srcSizes,
                                const int* __restrict srcOffsets, const int maxEvents, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) {
            const auto offset = srcOffsets[gid];
            const auto size = srcSizes[gid];

            for (int i = 0; i < size; ++i) {
                const auto idst = offset + i;
                const auto isrc = gid * maxEvents + i;
                srcIndices[idst] = isrc;
            }
        }
    }
};

struct GatherKernel {
    template <typename Acc, typename T>
    RAYX_FN_ACC void operator()(const Acc& __restrict acc, T* __restrict dst, const T* __restrict src, const int* __restrict srcIndices,
                                const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) {
            const auto isrc = srcIndices[gid];
            dst[gid] = src[isrc];
        }
    }
};

}  // namespace RAYX
