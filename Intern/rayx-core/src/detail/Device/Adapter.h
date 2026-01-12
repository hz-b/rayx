#pragma once

#include <complex>

#ifdef RAYX_CUDA_ENABLED
#include <cuda/std/complex>
#endif

namespace rayx::detail::device {

template <alpaka::concepts::DeviceKind T_DeviceKind>
struct ComplexOf;

template <>
struct ComplexOf<alpaka::deviceKind::cpu> {
    using type = std::complex<double>;
};

#ifdef RAYX_CUDA_ENABLED
template <>
struct ComplexOf<alpaka::deviceKind::nvidiaGpu> {
    using type = cuda::std::complex<double>;
};
#endif

template <alpaka::concepts::DeviceKind T_DeviceKind>
using complex_t = typename ComplexOf<T_DeviceKind>::type;

}  // namespace rayx::detail::device
