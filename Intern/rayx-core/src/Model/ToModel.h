#pragma once

#include <array>
#include <vector>

namespace rayx::detail {

inline int toModel(const int value) { return value; }

inline double toModel(const double value) { return value; }

template <std::size_t N>
inline auto toModel(const std::array<double, N>& arr) {
    return arr;
}

template <typename T, std::size_t N>
inline auto toModel(const std::array<T, N>& arr) {
    using R     = decltype(toModel(std::declval<T>()));
    auto result = std::array<R, N>();
    for (size_t i = 0; i < N; ++i) result[i] = toModel(arr[i]);
    return result;
}

inline auto toModel(const std::vector<double>& vec) { return vec; }

template <typename T>
inline auto toModel(const std::vector<T>& vec) {
    using R     = decltype(toModel(std::declval<T>()));
    auto result = std::vector<R>(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) result[i] = toModel(vec[i]);
    return result;
}

template <typename T>
inline auto toModel(const std::optional<T>& opt) -> std::optional<decltype(toModel(*opt))> {
    if (opt) return toModel(*opt);
    return std::nullopt;
}

template <typename ...Ts>
inline auto toModel(const std::variant<Ts...>& var) {
    return std::visit([](const auto& value) { return toModel(value); }, var);
}

}  // namespace rayx::detail
