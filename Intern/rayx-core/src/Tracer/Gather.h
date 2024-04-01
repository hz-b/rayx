#pragma once

#include "Common.h"
#include "Accelerator.h"

namespace RAYX {

struct GatherKernel {
    template <typename Acc, typename T>
    RAYX_FUNC
    void operator() (
        const Acc& acc,
        T* dst,
        const T* src,
        const alpaka::Idx<Acc>* srcOffsets,
        const alpaka::Idx<Acc>* srcSizes,
        const alpaka::Idx<Acc> srcMaxSize,
        const alpaka::Idx<Acc> n
    ) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) {
            using Idx = alpaka::Idx<Acc>;

            auto offset = srcOffsets[gid];
            auto size = srcSizes[gid];

            for (int i = 0; i < size; ++i) {
                const auto idst = offset + i;
                const auto isrc = gid * srcMaxSize + i;
                dst[idst] = src[isrc];
            }
        }
    }
};

template <
    typename Acc,
    typename T,
    typename Queue
>
inline void gather_n(
    Queue queue,
    Buf<Acc, T> dst,
    Buf<Acc, T> src,
    Buf<Acc, alpaka::Idx<Acc>> srcOffsets,
    Buf<Acc, alpaka::Idx<Acc>> srcSizes,
    const alpaka::Idx<Acc> srcMaxSize,
    const alpaka::Idx<Acc> n
) {
    alpaka::exec<Acc>(
        queue,
        getWorkDivForAcc<Acc>(n),
        GatherKernel{},
        alpaka::getPtrNative(dst),
        alpaka::getPtrNative(src),
        alpaka::getPtrNative(srcOffsets),
        alpaka::getPtrNative(srcSizes),
        srcMaxSize,
        n
    );
}

} // namespace RAYX
