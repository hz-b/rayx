#pragma once

////////////////////////////////////////////////////////////
// helper function unwrapArithmeticStrongTypedefIfPossible
////////////////////////////////////////////////////////////

namespace rayx::detail {

template <typename T>
concept ArithmeticStrongTypedef = requires(T t) {
    { t.value() } -> std::convertible_to<decltype(t.value())>;
    requires std::is_arithmetic_v<decltype(t.value())>;
};

// Base case: arithmetic primitives
template <typename T>
constexpr auto unwrapArithmeticStrongTypedefIfPossible(T&& value) {
    return value;
}

// Strong typedefs with value() method
template <ArithmeticStrongTypedef T>
constexpr auto unwrapArithmeticStrongTypedefIfPossible(T&& value) {
    return value.value();
}

// Variants of strong typedefs
template <ArithmeticStrongTypedef... Ts>
constexpr auto unwrapArithmeticStrongTypedefIfPossible(const std::variant<Ts...>& value) {
    return std::visit([](auto&& arg) { return unwrapArithmeticStrongTypedefIfPossible(arg); }, value);
}

}  // namespace rayx::detail
