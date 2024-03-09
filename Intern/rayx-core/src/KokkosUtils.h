#pragma once

#include <Kokkos_Core.hpp>

template <typename MemSpace>
struct KokkosUtils {

#ifndef NO_GPU_TRACER
    static constexpr bool transferRequired = std::is_same_v<Kokkos::CudaSpace, MemSpace>;
#else
    static constexpr bool transferRequired = false;
#endif

    template <typename T>
    static auto createView(const std::string& name, int size) {
        return Kokkos::View<T*, MemSpace>(name, size);
    }

    template <typename T>
    static auto createView(const std::string& name, int size, const T* data) {
        if constexpr (transferRequired) {
            // TODO(Sven): eliminate unnecessary copy before transfer
            // auto host_view = Kokkos::View<const T*, HostSpace>(data, size);
            auto view = createView<T>(name, size);
            auto host_view = Kokkos::create_mirror(view);
            std::memcpy(host_view.data(), data, size * sizeof(T));
            Kokkos::deep_copy(view, host_view);
            return view;
        } else {
            // TODO(Sven): eliminate unnecessary copy
            // auto view = Kokkos::View<const T*, MemSpace>(data, size);
            auto view = createView<T>(name, size);
            std::memcpy(view.data(), data, size * sizeof(T));
            return view;
        }
    }

    template <typename T>
    static auto createView(const std::string& name, const std::vector<T>& data) {
        return createView(name, data.size(), data.data());
    }

    template <typename T>
    static auto createVector(const Kokkos::View<T*, MemSpace> view) {
        auto v = std::vector<T>(view.size());

        if constexpr (transferRequired) {
            auto host_view = Kokkos::create_mirror(view);
            Kokkos::deep_copy(host_view, view);
            std::memcpy(v.data(), host_view.data(), host_view.size() * sizeof(T));
        } else {
            std::memcpy(v.data(), view.data(), view.size() * sizeof(T));
        }

        return v;
    }
};
