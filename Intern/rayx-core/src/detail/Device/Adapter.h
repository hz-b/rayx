#pragma once

#include <alpaka/alpaka.hpp>

#ifdef RAYX_CUDA_ENABLED
#include <cuda/std/complex>
#endif

#include "math/Complex.h"
#include "math/ElectricField.h"

namespace rayx::detail::device {

////////////////////////////////////////////////////////////////////////////////
// Complex
////////////////////////////////////////////////////////////////////////////////

template <alpaka::concepts::DeviceKind T_DeviceKind>
struct ComplexOf;

template <>
struct ComplexOf<alpaka::deviceKind::cpu> {
    using type = math::Complex;
};

#ifdef RAYX_CUDA_ENABLED
template <>
struct ComplexOf<alpaka::deviceKind::nvidiaGpu> {
    using type = cuda::std::complex<double>;
};
#endif

template <alpaka::concepts::DeviceKind T_DeviceKind>
using Complex_t = typename ComplexOf<T_DeviceKind>::type;
using Complex = Complex_t<alpaka::deviceKind::cpu>;

////////////////////////////////////////////////////////////////////////////////
// LocalElectricField
////////////////////////////////////////////////////////////////////////////////

template <alpaka::concepts::DeviceKind T_DeviceKind>
struct LocalElectricFieldOf;

template <>
struct LocalElectricFieldOf<alpaka::deviceKind::cpu> {
    using type = math::LocalElectricField;
}

#ifdef RAYX_CUDA_ENABLED
template <>
struct LocalElectricFieldOf<alpaka::deviceKind::nvidiaGpu> {
    using type = glm::tvec2<Complex_t<alpaka::deviceKind::nvidiaGpu>>;
}
#endif

template <alpaka::concepts::DeviceKind T_DeviceKind>
using LocalElectricField_t = typename LocalElectricFieldOf<T_DeviceKind>::type;
using LocalElectricField = LocalElectricField_t<alpaka::deviceKind::cpu>;

////////////////////////////////////////////////////////////////////////////////
// ElectricField
////////////////////////////////////////////////////////////////////////////////

template <alpaka::concepts::DeviceKind T_DeviceKind>
struct ElectricFieldOf;

template <>
struct ElectricFieldOf<alpaka::deviceKind::cpu> {
    using type = math::ElectricField;
}

#ifdef RAYX_CUDA_ENABLED
template <>
struct ElectricFieldOf<alpaka::deviceKind::nvidiaGpu> {
    using type = glm::tvec3<Complex_t<alpaka::deviceKind::nvidiaGpu>>;
}
#endif

template <alpaka::concepts::DeviceKind T_DeviceKind>
using ElectricField_t = typename ElectricFieldOf<T_DeviceKind>::type;

}  // namespace rayx::detail::device
