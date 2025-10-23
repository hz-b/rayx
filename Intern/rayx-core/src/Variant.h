#pragma once

#include "Core.h"

#if defined(RAYX_CUDA_ENABLED)
#include <cuda/std/variant>
#else
#include <variant>
#endif

#if defined(RAYX_CUDA_ENABLED)
namespace variant = cuda::std;
#else
namespace variant = std;
#endif

namespace RAYX {

/**
 * @brief A variant class that extends a base class and holds a variant of types Ts.
 * @tparam Base The base class to extend, which can provide structs to be used as types in the variant.
 * @tparam Ts The types that the variant can hold.
 * @example
 * ```cpp
 * struct MyTypes {
 *     struct TypeA { int a; };
 *     struct TypeB { double b; };
 * };
 * using MyVariant = RAYX::Variant<MyTypes, MyTypes::TypeA, MyTypes::TypeB>;
 * // or if you want to extend functionality
 * struct ExtendedBase : public RAYX::Variant<MyTypes, MyTypes::TypeA, MyTypes::TypeB> {
 *    // Additional functionality
 * };
 * ```
 */
template <typename Base, typename... Ts>
class Variant : public Base {
  public:
    Variant() = default;

    Variant(const Variant&) = default;
    Variant(Variant&&)      = default;

    Variant& operator=(const Variant&) = default;
    Variant& operator=(Variant&&)      = default;

    template <typename T>
        requires(std::is_constructible_v<variant::variant<Ts...>, T>)
    Variant(const T& value) : m_variant(value) {}

    template <typename T>
        requires(std::is_constructible_v<variant::variant<Ts...>, T>)
    Variant(T&& value) : m_variant(std::move(value)) {}

    template <typename T>
        requires(std::is_assignable_v<variant::variant<Ts...>&, const T&>)
    Variant& operator=(const T& other) {
        m_variant = other;
        return *this;
    }

    template <typename T>
        requires(std::is_assignable_v<variant::variant<Ts...>&, T>)
    Variant& operator=(T&& value) {
        m_variant = std::move(value);
        return *this;
    }

    template <typename T>
    RAYX_FN_ACC bool is() const {
        return variant::holds_alternative<T>(m_variant);
    }

    template <typename T>
    RAYX_FN_ACC T& get() {
        return variant::get<T>(m_variant);
    }

    template <typename T>
    RAYX_FN_ACC const T& get() const {
        return variant::get<T>(m_variant);
    }

    template <typename Visitor>
    RAYX_FN_ACC decltype(auto) visit(const Visitor& visitor) const {
        return variant::visit(visitor, m_variant);
    }

    template <typename Visitor>
    RAYX_FN_ACC decltype(auto) visit(Visitor&& visitor) const {
        return variant::visit(std::forward<Visitor>(visitor), m_variant);
    }

  private:
    variant::variant<Ts...> m_variant;
};

}  // namespace RAYX
