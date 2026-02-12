#pragma once

#include <cmath>
#include <format>
#include <glm/glm.hpp>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "StrongTypedef.h"

namespace rayx::detail {

////////////////////////////////////////////////////////////
// validation functions
////////////////////////////////////////////////////////////

template <typename T>
    requires std::is_arithmetic_v<T>
constexpr bool nearlyEqual(const T a, const T b, const T epsilon = std::numeric_limits<T>::epsilon() * T(1000)) {
    return std::abs(a - b) < epsilon;
}

constexpr inline std::string formatNote(const std::optional<std::string_view>& note) {
    if (note) return std::format(". note: {}", *note);
    return "";
}

template <typename T>
    requires std::is_arithmetic_v<T>
constexpr void validateGreaterEqualZero(const std::string_view typeName, const std::string_view propertyName, const T value,
                                        const std::optional<std::string_view> note = std::nullopt) {
    if (value < 0.0) [[unlikely]] {
        throw std::invalid_argument(std::format("error: property `{}` of `{}` must be greater or equal to `0`. got: `{}`{}", propertyName, typeName,
                                                value, formatNote(note)));
    }
}

template <typename T>
    requires std::is_arithmetic_v<T>
constexpr void validateGreaterZero(const std::string_view typeName, const std::string_view propertyName, const T value,
                                   const std::optional<std::string_view> note = std::nullopt) {
    if (value <= 0.0) [[unlikely]] {
        throw std::invalid_argument(
            std::format("error: property `{}` of `{}` must be greater than `0`. got: `{}`{}", propertyName, typeName, value, formatNote(note)));
    }
}

template <typename T>
    requires std::is_arithmetic_v<T>
constexpr void validateInRange(const std::string_view typeName, const std::string_view propertyName, const T value, const T minValue,
                               const T maxValue, const std::optional<std::string_view> note = std::nullopt) {
    if (value < minValue || value > maxValue) [[unlikely]] {
        throw std::invalid_argument(std::format("error: property `{}` of `{}` must be in range [`{}`, `{}`]. got: `{}`{}", propertyName, typeName,
                                                minValue, maxValue, value, formatNote(note)));
    }
}

template <typename T>
    requires std::is_arithmetic_v<T>
constexpr void validateEqual(const std::string_view typeName, const std::string_view propertyName, const T value, const T expectedValue,
                             const std::optional<std::string_view> note = std::nullopt) {
    if (!nearlyEqual(value, expectedValue)) [[unlikely]] {
        throw std::invalid_argument(std::format("error: property `{}` of `{}` must be equal to `{}`. got: `{}`{}", propertyName, typeName,
                                                expectedValue, value, formatNote(note)));
    }
}

constexpr void validateVec3Orthogonal(const std::string_view typeName, const std::string_view vectorAName, const glm::dvec3& vectorA,
                                      const std::string_view vectorBName, const glm::dvec3& vectorB,
                                      const std::optional<std::string_view> note = std::nullopt) {
    auto cosRad = glm::dot(vectorA, vectorB);
    if (!nearlyEqual(cosRad, 0.0)) [[unlikely]] {
        throw std::invalid_argument(std::format("error: vectors `{}` and `{}` of `{}` must be orthogonal. got dot product: `{}`{}", vectorAName,
                                                vectorBName, typeName, cosRad, formatNote(note)));
    }
}

template <typename T>
constexpr void validateVectorSizeGreaterZero(const std::string_view typeName, const std::string_view vectorName, const std::vector<T>& vector,
                                             const std::optional<std::string_view> note = std::nullopt) {
    if (vector.size() == 0) [[unlikely]] {
        throw std::invalid_argument(std::format("error: vector `{}` of `{}` must have size greater than `0`. got: `{}`{}", vectorName, typeName,
                                                vector.size(), formatNote(note)));
    }
}

template <typename T, typename U>
constexpr void validateVectorSizesEqual(const std::string_view typeName, const std::string_view vectorAName, const std::vector<T>& vectorA,
                                        const std::string_view vectorBName, const std::vector<U>& vectorB,
                                        const std::optional<std::string_view> note = std::nullopt) {
    if (vectorA.size() != vectorB.size()) [[unlikely]] {
        throw std::invalid_argument(std::format("error: vectors `{}` and `{}` of `{}` must have the same size. got: `{}` and `{}`{}", vectorAName,
                                                vectorBName, typeName, vectorA.size(), vectorB.size(), formatNote(note)));
    }
}

}  // namespace rayx::detail

////////////////////////////////////////////////////////////
// macros for properties
//
// avoids code duplication
// ensures validation
// ensures consistent error messages
// validation and nesting can not be used together, as validation requires the getter to return by value, while nesting
// requires the getter to return by reference
////////////////////////////////////////////////////////////

namespace rayx::detail {

// helper trait to determine which types are allowed to be validated
// all validated proerties must be returned by value by the getter, as validation is performed in the setter.
// returning by reference would allow bypassing validation and thus is not allowed for validated properties.
template <typename T>
struct is_property_validatable : std::false_type {};

// all arithmetic and enum types can safely be returned by value
template <typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
struct is_property_validatable<T> : std::true_type {};

// all std::optional of arithmetic and enum types can safely be returned by value
template <typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
struct is_property_validatable<std::optional<T>> : std::true_type {};

// by convention all std::vector properties are returned by value
template <typename T>
struct is_property_validatable<std::vector<T>> : std::true_type {};

// std::vector is allowed to be optional
template <typename T>
struct is_property_validatable<std::optional<std::vector<T>>> : std::true_type {};

// by convention all std::array properties are returned by value
template <typename T, std::size_t N>
struct is_property_validatable<std::array<T, N>> : std::true_type {};

// std::array is allowed to be optional
template <typename T, std::size_t N>
struct is_property_validatable<std::optional<std::array<T, N>>> : std::true_type {};

template <typename T>
constexpr bool is_property_validatable_v = is_property_validatable<T>::value;

// helper trait to determine which types are allowed to be nested
// all nestable properties must be returned by reference by the getter,
// as nesting requires the user to be able to modify the nested properties.
// this is only the possible if no validation is performed in the setter.
template <typename T>
struct is_property_nestable : std::true_type {};

// by convention all std::vector properties are not nestable
template <typename T>
struct is_property_nestable<std::vector<T>> : std::false_type {};

// std::vector is not allowed to be optional, as it is already not nestable
template <typename T>
struct is_property_nestable<std::optional<std::vector<T>>> : std::false_type {};

// by convention all std::array properties are not nestable
template <typename T, std::size_t N>
struct is_property_nestable<std::array<T, N>> : std::false_type {};

// std::array is not allowed to be optional, as it is already not nestable
template <typename T, std::size_t N>
struct is_property_nestable<std::optional<std::array<T, N>>> : std::false_type {};

template <typename T>
constexpr bool is_property_nestable_v = is_property_nestable<T>::value;

}  // namespace rayx::detail

#define RAYX_COMMA ,

// TODO: maybe we do not need RAYX_PROPERTY and just use RAYX_NESTED_PROPERTY for primitives. returning by reference is
// not that dangerous anyways
#define RAYX_PROPERTY(classType, propertyType, propertyName)                                            \
    static_assert(detail::is_property_validatable_v<propertyType>,                                      \
                  "RAYX_PROPERTY only supports arithmetic, enum, std::vector and std::optional types"); \
                                                                                                        \
  public:                                                                                               \
    constexpr propertyType propertyName() const { return m_##propertyName; }                            \
    constexpr classType& propertyName(propertyType value) {                                             \
        m_##propertyName = std::move(value);                                                            \
        return *this;                                                                                   \
    }                                                                                                   \
                                                                                                        \
  private:                                                                                              \
    propertyType m_##propertyName

#define RAYX_VALIDATED_PROPERTY(classType, propertyType, propertyName, validationFunc)                                \
    static_assert(detail::is_property_validatable_v<propertyType>,                                                    \
                  "RAYX_PROPERTY only supports arithmetic, enum, std::vector and std::optional types");               \
                                                                                                                      \
  public:                                                                                                             \
    constexpr propertyType propertyName() const { return m_##propertyName; }                                          \
    constexpr classType& propertyName(propertyType value) {                                                           \
        validationFunc(#classType, #propertyName, detail::unwrapArithmeticStrongTypedefIfPossible(value));            \
        m_##propertyName = std::move(value);                                                                          \
        return *this;                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
  private:                                                                                                            \
    constexpr void validate_##propertyName() {                                                                        \
        validationFunc(#classType, #propertyName, detail::unwrapArithmeticStrongTypedefIfPossible(m_##propertyName)); \
    }                                                                                                                 \
    propertyType m_##propertyName

#define RAYX_NESTED_PROPERTY(classType, propertyType, propertyName)                                                         \
    static_assert(detail::is_property_nestable_v<propertyType>, "RAYX_NESTED_PROPERTY does not support std::vector types"); \
                                                                                                                            \
  public:                                                                                                                   \
    constexpr const propertyType& propertyName() const { return m_##propertyName; }                                         \
    constexpr propertyType& propertyName() { return m_##propertyName; }                                                     \
    constexpr classType& propertyName(propertyType value) {                                                                 \
        m_##propertyName = std::move(value);                                                                                \
        return *this;                                                                                                       \
    }                                                                                                                       \
                                                                                                                            \
  private:                                                                                                                  \
    propertyType m_##propertyName
