#pragma once

#include "Accelerator.h"
#include "Common.h"

namespace RAYX {

// TODO(Sven): implement parallel scan
template <typename Acc, typename T, typename Queue>
inline alpaka::Idx<Acc> scanSum(Queue queue, Buf<Acc, T> dst, Buf<Acc, T> src, const alpaka::Idx<Acc> n) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    using Cpu = alpaka::DevCpu;
    using Seq = alpaka::AccCpuSerial<Dim, Idx>;

    auto cpu = getDevice<Cpu>(0);
    auto seq = getDevice<Seq>(0);

    auto h_src = alpaka::allocBuf<T, Idx>(cpu, Vec{n});
    auto h_dst = alpaka::allocBuf<T, Idx>(cpu, Vec{n});

    alpaka::memcpy(queue, h_src, src, Vec{n});

    Idx sum;
    alpaka::exec<Seq>(
        queue,
        alpaka::WorkDivMembers<Dim, Idx>{
            Vec{1},
            Vec{1},
            Vec{1},
        },
        [] ALPAKA_FN_HOST(const auto&, Idx* sum, T* dst, const T* src, const Idx n) {
            *sum = 0;
            for (Idx i = 0; i < n; ++i) {
                dst[i] = *sum;
                *sum += src[i];
            }
        },
        &sum, alpaka::getPtrNative(h_dst), alpaka::getPtrNative(h_src), n);

    // wait because the above kernel is not guaranteed to finish before the next task
    alpaka::wait(queue);

    alpaka::memcpy(queue, dst, h_dst, Vec{n});

    // wait for all operations to finish, before destroying buffers
    alpaka::wait(queue);

    return sum;
}

}  // namespace RAYX
