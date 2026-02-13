#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <variant>

////////////////////////////////////////////////////////////
// type definitions
////////////////////////////////////////////////////////////

namespace rayx {

struct UnitVec3 {
    constexpr UnitVec3(glm::dvec3 vec) : m_vec(vec) { detail::validateEqual("UnitVec3", "length", glm::length(vec), 1.0); }
    constexpr UnitVec3(double x, double y, double z) : UnitVec3(glm::dvec3(x, y, z)) {}

    constexpr double x() const { return m_vec.x; }
    constexpr double y() const { return m_vec.y; }
    constexpr double z() const { return m_vec.z; }
    constexpr glm::dvec3 toVec3() const { return m_vec; }

  private:
    glm::dvec3 m_vec;
};

// essentially a quaternion rotation (glm::dquat)
struct RotationAroundAxis {
    constexpr RotationAroundAxis(Angle angle, UnitVec3 axis) : m_angle(angle), m_axis(axis) {}

    RAYX_NESTED_PROPERTY(RotationAroundAxis, Angle, angle);
    RAYX_NESTED_PROPERTY(RotationAroundAxis, UnitVec3, axis) = { 0.0, 0.0, 1.0 };
};

struct RotationBase {
    constexpr RotationBase(UnitVec3 right, UnitVec3 up, UnitVec3 forward) : m_right(right), m_up(up), m_forward(forward) {
        detail::validateVec3Orthogonal("RotationBase", "right", right.toVec3(), "up", up.toVec3());
        detail::validateVec3Orthogonal("RotationBase", "right", right.toVec3(), "forward", forward.toVec3());
        detail::validateVec3Orthogonal("RotationBase", "up", up.toVec3(), "forward", forward.toVec3());
    }

    constexpr UnitVec3 right() const { return m_right; }
    constexpr UnitVec3 up() const { return m_up; }
    constexpr UnitVec3 forward() const { return m_forward; }

  private:
    UnitVec3 m_right;
    UnitVec3 m_up;
    UnitVec3 m_forward;
};

using Rotation = std::variant<RotationAroundAxis, RotationBase, glm::dmat3>;

}  // namespace rayx

////////////////////////////////////////////////////////////
// conversion functions
////////////////////////////////////////////////////////////

namespace rayx {

constexpr inline RotationAroundAxis toRotationAroundAxis(const RotationAroundAxis& rotation) { return rotation; }

constexpr inline RotationAroundAxis toRotationAroundAxis(const RotationBase& rotation) {
    const auto quat = glm::quat_cast(glm::dmat3(rotation.right().toVec3(), rotation.up().toVec3(), rotation.forward().toVec3()));
    return RotationAroundAxis(Rad{glm::angle(quat)}, glm::axis(quat));
}

constexpr inline RotationAroundAxis toRotationAroundAxis(const glm::dmat3& rotation) {
    const auto quat = glm::quat_cast(rotation);
    return RotationAroundAxis(Rad{glm::angle(quat)}, glm::axis(quat));
}

constexpr inline RotationAroundAxis toRotationAroundAxis(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationAroundAxis(arg); }, rotation);
}

constexpr inline RotationBase toRotationBase(const RotationBase& rotation) { return rotation; }

constexpr inline RotationBase toRotationBase(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRad(rotation.angle()).value(), rotation.axis().toVec3());
    const auto mat  = glm::mat3_cast(quat);
    return RotationBase(mat[0], mat[1], mat[2]);
}

constexpr inline RotationBase toRotationBase(const glm::dmat3& rotation) { return RotationBase(rotation[0], rotation[1], rotation[2]); }

constexpr inline RotationBase toRotationBase(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationBase(arg); }, rotation);
}

constexpr inline glm::dmat3 toMatrix(const glm::dmat3& rotation) { return rotation; }

constexpr inline glm::dmat3 toMatrix(const RotationBase& rotation) {
    return glm::dmat3(rotation.right().toVec3(), rotation.up().toVec3(), rotation.forward().toVec3());
}

constexpr inline glm::dmat3 toMatrix(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRad(rotation.angle()).value(), rotation.axis().toVec3());
    return glm::mat3_cast(quat);
}

constexpr inline glm::dmat3 toMatrix(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toMatrix(arg); }, rotation);
}

}  // namespace rayx

////////////////////////////////////////////////////////////
// std::ostream operator<< overloads
////////////////////////////////////////////////////////////

#include <ostream>

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::UnitVec3& unitVec) {
    return os << "{" << unitVec.x() << ", " << unitVec.y() << ", " << unitVec.z() << "}";
}

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::RotationAroundAxis& rotation) {
    return os << "{angle=" << rotation.angle() << ", axis=" << rotation.axis() << "}";
}

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::RotationBase& rotation) {
    return os << "{right=" << rotation.right() << ", up=" << rotation.up() << ", forward=" << rotation.forward() << "}";
}

constexpr inline std::ostream& operator<<(std::ostream& os, const glm::dmat3& rotation) {
    os << "[";
    for (int i = 0; i < 3; i++) {
        os << "{" << rotation[i][0] << ", " << rotation[i][1] << ", " << rotation[i][2] << "}";
        if (i < 2) os << ", ";
    }
    os << "]";
    return os;
}

constexpr inline std::ostream& operator<<(std::ostream& os, const rayx::Rotation& rotation) {
    return std::visit([&os](const auto& arg) -> std::ostream& { return os << arg; }, rotation);
}

////////////////////////////////////////////////////////////
// std::formatter specializations
////////////////////////////////////////////////////////////

// TODO: implement
