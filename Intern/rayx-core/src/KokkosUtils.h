#pragma once

#include <Kokkos_Core.hpp>

template <typename KokkosMemorySpace>
struct KokkosUtils {
    static constexpr bool transferRequired =
        std::is_same_v<Kokkos::CudaSpace, KokkosMemorySpace>
        // || std::is_same_v<Kokkos::HIPSpace, KokkosMemorySpace>
    ;

    template <typename T>
    static auto createView(const std::string& name, int size) {
        return Kokkos::View<T*, KokkosMemorySpace>(name, size);
    }

    template <typename T>
    static auto createView(const std::string& name, int size, const T* data) {
        auto view = createView<T>(name, size);

        if constexpr (transferRequired) {
            auto host_view = Kokkos::create_mirror(view);
            std::memcpy(host_view.data(), data, size * sizeof(T));
            Kokkos::deep_copy(view, host_view);
        } else {
            std::memcpy(view.data(), data, size * sizeof(T));
        }

        return view;
    }

    template <typename T>
    static auto createView(const std::string& name, const std::vector<T>& data) {
        return createView(name, data.size(), data.data());
    }

    template <typename T>
    static auto createVector(const Kokkos::View<T*, KokkosMemorySpace> view) {
        auto v = std::vector<T>(view.size());

        if constexpr (transferRequired) {
            auto host_view = Kokkos::create_mirror(view);
            Kokkos::deep_copy(view, host_view);
            std::memcpy(v.data(), host_view.data(), host_view.size() * sizeof(T));
        } else {
            std::memcpy(v.data(), view.data(), view.size() * sizeof(T));
        }

        return v;
    }
};
