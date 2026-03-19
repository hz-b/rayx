#pragma once

#include <array>
#include <optional>
#include <vector>

namespace rayx::detail {

////////////////////////////////////////////////////////////
// ToModel trait declaration
////////////////////////////////////////////////////////////

template <typename T>
struct ToModel;

////////////////////////////////////////////////////////////
// generic toModel function that dispatches to the appropriate ToModel specialization
////////////////////////////////////////////////////////////

template <typename T>
inline auto toModel(const T& value) {
    return ToModel<T>::apply(value);
}

////////////////////////////////////////////////////////////
// ToModel trait specializations for basic types and containers
////////////////////////////////////////////////////////////

template <>
struct ToModel<bool> {
    static auto apply(const bool value) { return value; }
};

template <>
struct ToModel<int> {
    static auto apply(const int value) { return value; }
};

template <>
struct ToModel<double> {
    static auto apply(const double value) { return value; }
};

template <>
struct ToModel<std::vector<double>> {
    static auto apply(const std::vector<double>& vec) { return vec; }
};

template <typename T>
struct ToModel<std::vector<T>> {
    static auto apply(const std::vector<T>& vec) {
        using ElementType = decltype(toModel(std::declval<T>()));
        auto result       = std::vector<ElementType>();
        result.reserve(vec.size());
        for (const auto& item : vec) result.push_back(toModel(item));
        return result;
    }
};

template <std::size_t N>
struct ToModel<std::array<double, N>> {
    static auto apply(const std::array<double, N>& arr) { return arr; }
};

template <typename T, std::size_t N>
struct ToModel<std::array<T, N>> {
    static auto apply(const std::array<T, N>& arr) {
        using ElementType = decltype(toModel(std::declval<T>()));
        auto result       = std::array<ElementType, N>();
        for (size_t i = 0; i < N; ++i) result[i] = toModel(arr[i]);
        return result;
    }
};

template <typename T>
struct ToModel<std::optional<T>> {
    static auto apply(const std::optional<T>& opt) -> std::optional<decltype(toModel(*opt))> {
        if (opt) return toModel(*opt);
        return std::nullopt;
    }
};

}  // namespace rayx::detail
