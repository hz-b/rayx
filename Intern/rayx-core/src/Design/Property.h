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

namespace rayx::detail {

// properties
// - all members of design layer structs/classes must be properties
//   this allows us to enforce consistent
//   - convetions
//   - validation
//   - error messages
//   - code duplication.
// - a property can either be validatable or nestable
//
// validatable properties
// - must be returned by value by the getter, as validation is performed in the setter.
//   returning by reference would allow bypassing validation and thus is not allowed for validated properties.
// - allowed types:
//   - arithmetic types (bool, int, double, etc.)
//   - enum types
//   - std::array
//   - std::vector
//   - std::optional of arithmetic, enum, std::array or std::vector types
//
// nestable properties
// - must be returned by reference by the getter, as nesting requires the user to be able to modify the nested properties.
//   returning by value would not allow modifying the already existing instance of the nested property
// - allows any type that is not a validatable property

// helper trait to determine which types are allowed to be validated
template <typename T>
struct is_property_validatable : std::false_type {};

// all arithmetic and enum types can safely be returned by value
template <typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
struct is_property_validatable<T> : std::true_type {};

// by convention all std::vector properties are returned by value
template <typename T>
struct is_property_validatable<std::vector<T>> : std::true_type {};

// by convention all std::array properties are returned by value
template <typename T, std::size_t N>
struct is_property_validatable<std::array<T, N>> : std::true_type {};

// all std::optional of validatable types are also validatable
template <typename T>
    requires is_property_validatable<T>::value
struct is_property_validatable<std::optional<T>> : std::true_type {};

/// test T for being a validatable property type
template <typename T>
constexpr bool is_property_validatable_v = is_property_validatable<T>::value;

// helper trait to determine which types are allowed to be nested
template <typename T>
struct is_property_nestable : std::false_type {};

// explicitly allow all types that are not validatable to be nested, as they cannot be returned by value and thus must be returned by reference
// allow all arithmetic and enum types to be nested, when they do not require validation
template <typename T>
    requires (!is_property_validatable_v<T> || std::is_arithmetic_v<T> || std::is_enum_v<T>)
struct is_property_nestable<T> : std::true_type {};

/// test T for being a nestable property type
template <typename T>
constexpr bool is_property_nestable_v = is_property_nestable<T>::value;

}  // namespace rayx::detail

////////////////////////////////////////////////////////////
// macros for properties
//
// avoids code duplication
// ensures consistent validation
// ensures consistent error messages
////////////////////////////////////////////////////////////

#define RAYX_COMMA ,

// TODO: maybe we do not need RAYX_PROPERTY and just use RAYX_NESTED_PROPERTY for primitives. returning by reference is
// not that dangerous anyways

/// defines a property with the given name and type in the given class.
/// the property must be validatable, otherwise use RAYX_NESTED_PROPERTY.
/// this macro is basically the same as RAYX_VALIDATED_PROPERTY, but without validation.
/// it is useful for properties that do not require validation.
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

/// defines a property with the given name and type in the given class.
/// the property must be validatable, otherwise use RAYX_NESTED_PROPERTY.
/// the setter performs validation using the given validation function.
#define RAYX_VALIDATED_PROPERTY(classType, propertyType, propertyName, validationFunc)                                        \
    static_assert(detail::is_property_validatable_v<propertyType>,                                                            \
                  "RAYX_PROPERTY only supports arithmetic, enum, std::vector and std::optional types");                       \
                                                                                                                              \
  public:                                                                                                                     \
    constexpr propertyType propertyName() const { return m_##propertyName; }                                                  \
    constexpr classType& propertyName(propertyType value) {                                                                   \
        validationFunc(#classType, #propertyName, value);                                                     \
        m_##propertyName = std::move(value);                                                                                  \
        return *this;                                                                                                         \
    }                                                                                                                         \
                                                                                                                              \
  private:                                                                                                                    \
    constexpr void validate_##propertyName() { validationFunc(#classType, #propertyName, m_##propertyName); } \
    propertyType m_##propertyName

/// defines a property with the given name and type in the given class.
/// the property must be nestable, otherwise use RAYX_PROPERTY.
#define RAYX_NESTED_PROPERTY(classType, propertyType, propertyName)                                                                        \
    static_assert(detail::is_property_nestable_v<propertyType>, "RAYX_NESTED_PROPERTY does not support std::vector and std::array types"); \
                                                                                                                                           \
  public:                                                                                                                                  \
    constexpr const propertyType& propertyName() const { return m_##propertyName; }                                                        \
    constexpr propertyType& propertyName() { return m_##propertyName; }                                                                    \
    constexpr classType& propertyName(propertyType value) {                                                                                \
        m_##propertyName = std::move(value);                                                                                               \
        return *this;                                                                                                                      \
    }                                                                                                                                      \
                                                                                                                                           \
  private:                                                                                                                                 \
    propertyType m_##propertyName
