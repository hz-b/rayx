#pragma once

#include <Kokkos_Core.hpp>

template <typename KokkosMemorySpace>
struct KokkosUtils {
    template <typename T>
    static auto createView(const std::string& name, int size) {
        return Kokkos::View<T*, KokkosMemorySpace>(name, size);
    }

    template <typename T>
    static auto createView(const std::string& name, int size, const T* data) {
        auto view = createView<T>(name, size);

        constexpr auto transferToDevice = !std::is_same_v<Kokkos::HostSpace, KokkosMemorySpace>;
        if constexpr (transferToDevice) {
            // TODO: implement
            RAYX_ERR << "Transfer to Device is not zet implemented.";
        } else {
            std::memcpy(view.data(), data, size * sizeof(T));
        }

        return view;
    }

    template <typename T>
    static auto createView(const std::string& name, const std::vector<T>& data) {
        return createView(name, data.size(), data.data());
    }
};
