#pragma once

#include <alpaka/alpaka.hpp>

namespace RAYX {

template <typename Acc, typename T>
using Buf = alpaka::Buf<Acc, T, alpaka::Dim<Acc>, alpaka::Idx<Acc>>;

}  // namespace RAYX
