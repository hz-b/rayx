#pragma once

#include <alpaka/alpaka.hpp>
#include <optional>
#include <vector>

#include "Debug/Instrumentor.h"
#include "Shader/Rand.h"
#include "Shader/RaysPtr.h"

namespace RAYX {

template <typename Acc, typename T>
using OptBuf = std::optional<alpaka::Buf<Acc, T, alpaka::DimInt<1>, int32_t>>;

// this struct is analog to struct Rays. It contains OptBufs instead of vectors, so it can be used as buffers on CPU or GPU
template <typename Acc>
struct RaysBuf {
#define X(type, name, flag) OptBuf<Acc, type> name;

    RAYX_X_MACRO_RAY_ATTR
#undef X
};

template <typename Acc>
RaysPtr raysBufToRaysPtr(RaysBuf<Acc>& buf) {
    return RaysPtr{
#define X(type, name, flag) .name = buf.name ? alpaka::getPtrNative(*buf.name) : nullptr,

        RAYX_X_MACRO_RAY_ATTR
#undef X
    };
}

inline int ceilIntDivision(const int dividend, const int divisor) { return (divisor + dividend - 1) / divisor; }

inline int nextPowerOfTwo(const int value) { return static_cast<int>(glm::pow(2, glm::ceil(glm::log(value) / glm::log(2)))); }

/// conditionally allocate buffer with specified minimum size.
/// if the buffer already fulfills size requirements, this function does nothing.
/// this function never shrinks a buffer.
/// actual allocation size is nextPowerOfTwo(size).
/// this function is designed to optimize the repetitive use of the buffer with potentially different size requirements (e.g. tracing multiple
/// beamlines one after the other)
template <typename Queue, typename Buf>
inline void allocBuf(Queue q, std::optional<Buf>& buf, const int size) {
    using Idx  = alpaka::Idx<Buf>;
    using Elem = alpaka::Elem<Buf>;

    const auto shouldAlloc = !buf || alpaka::getExtents(*buf)[0] < size;
    if (shouldAlloc) RAYX_VERB << (!buf ? "new alloc on device: " : "realloc on device: ") << nextPowerOfTwo(size * sizeof(Elem)) << " bytes";
    if (shouldAlloc) buf = alpaka::allocAsyncBufIfSupported<Elem, Idx>(q, nextPowerOfTwo(size));
}

inline void collectCompactEventsIntoBundleHistory(BundleHistory& bundleHistory, const std::vector<Ray>& compactEvents,
                                                  const std::vector<int>& compactEventCounts, const std::vector<int>& compactEventOffsets,
                                                  const int batchSize) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (int i = 0; i < batchSize; i++) {
        const auto begin = compactEvents.data() + compactEventOffsets[i];
        const auto end   = begin + compactEventCounts[i];

        // add events to history, only if there are events
        bool hasEvents = 0 < std::distance(begin, end);
        if (hasEvents) bundleHistory.emplace_back(begin, end);
    }
}

namespace BlockSizeConstraint {

struct None {};

struct Exact {
    int value;
};

struct AtLeast {
    int value;
};

struct AtMost {
    int value;
};

struct InRange {
    int atLeast;
    int atMost;
};

using Variant = std::variant<None, Exact, AtLeast, AtMost, InRange>;

}  // namespace BlockSizeConstraint

// TODO: maybe make a PR to alpaka for alpaka::Acc<Dev> to extract Acc from DevAcc (= Dev<Platform<Acc>>)
template <typename Acc, typename DevAcc, typename Queue, typename Kernel, typename... Args>
inline void execWithValidWorkDiv(DevAcc devAcc, Queue q, const int numElements, BlockSizeConstraint::Variant blockSizeConstraint,
                                 const Kernel& kernel, Args&&... args) {
    const auto conf = alpaka::KernelCfg<Acc>{
        .gridElemExtent                        = numElements,
        .threadElemExtent                      = 1,
        .blockThreadMustDivideGridThreadExtent = false,
    };

    auto workDiv = alpaka::getValidWorkDiv(conf, devAcc, kernel, std::forward<Args>(args)...);
    std::visit(
        [&]<typename BlockSizeConstraintType>(BlockSizeConstraintType constraint) {
            if constexpr (std::is_same_v<BlockSizeConstraintType, BlockSizeConstraint::Exact>) {
                assert(workDiv.m_blockThreadExtent[0] <= constraint.value && "BlockSizeConstraint::Exact exceeds the capabilities this device");
                workDiv.m_blockThreadExtent = constraint.value;
                workDiv.m_gridBlockExtent   = ceilIntDivision(numElements, constraint.value);
            }

            if constexpr (std::is_same_v<BlockSizeConstraintType, BlockSizeConstraint::AtMost>) {
                if (constraint.value < workDiv.m_blockThreadExtent[0]) {
                    workDiv.m_blockThreadExtent = constraint.value;
                    workDiv.m_gridBlockExtent   = ceilIntDivision(numElements, constraint.value);
                }
            }

            if constexpr (std::is_same_v<BlockSizeConstraintType, BlockSizeConstraint::AtLeast>) {
                assert(constraint.value <= workDiv.m_blockThreadExtent[0] && "BlockSizeConstraint::AtLeast exceeds the capabilities this device");
            }

            if constexpr (std::is_same_v<BlockSizeConstraintType, BlockSizeConstraint::InRange>) {
                assert(constraint.atLeast <= workDiv.m_blockThreadExtent[0] && "BlockSizeConstraint::InRange exceeds capabilities of this device");
                if (constraint.atMost < workDiv.m_blockThreadExtent[0]) {
                    workDiv.m_blockThreadExtent = constraint.atMost;
                    workDiv.m_gridBlockExtent   = ceilIntDivision(numElements, constraint.atMost);
                }
            }
        },
        blockSizeConstraint);

    RAYX_VERB << "execute kernel with launch config: "
              << "blocks = " << workDiv.m_gridBlockExtent[0] << ", "
              << "threads = " << workDiv.m_blockThreadExtent[0] << ", "
              << "elements = " << workDiv.m_threadElemExtent[0];

    alpaka::exec<Acc>(q, workDiv, kernel, std::forward<Args>(args)...);
}

}  // namespace RAYX
