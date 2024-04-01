#pragma once

#include <alpaka/alpaka.hpp>

namespace RAYX {

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

template <typename T>
using printTypeAsCompileError = typename T::printTypeAsCompileError;

} // namespace RAYX
