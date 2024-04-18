#pragma once

#include <alpaka/alpaka.hpp>

namespace RAYX {

template <typename TBuf>
inline auto bufToSpan(TBuf& buf) {
    return std::span(alpaka::getPtrNative(buf), alpaka::getExtents(buf)[0]);
}

template <typename TBuf>
inline auto bufToSpan(TBuf& buf, const alpaka::Idx<TBuf> size) {
    return std::span(alpaka::getPtrNative(buf), size);
}

template <typename T>
using printTypeAsCompileError = typename T::printTypeAsCompileError;

} // namespace RAYX
