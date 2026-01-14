#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <variant>

namespace rayx::design {

// essentially a quaternion rotation (glm::dquat)
struct RotationAroundAxis {
    Angle angle;
    glm::dvec3 axis = {0.0, 0.0, 1.0};
};

struct RotationBase {
    glm::dvec3 right;
    glm::dvec3 up;
    glm::dvec3 forward;
};

using Rotation = std::variant<RotationAroundAxis, RotationBase, glm::dmat3>;

inline RotationAroundAxis toRotationAroundAxis(const RotationAroundAxis& rotation) { return rotation; }

inline RotationAroundAxis toRotationAroundAxis(const RotationBase& rotation) {
    const auto quat = glm::quat_cast(glm::dmat3(rotation.right, rotation.up, rotation.forward));
    return RotationAroundAxis{
        .angle = Radians{glm::angle(quat)},
        .axis  = glm::axis(quat),
    };
}

inline RotationAroundAxis toRotationAroundAxis(const glm::dmat3& rotation) {
    const auto quat = glm::quat_cast(rotation);
    return RotationAroundAxis{
        .angle = Radians{glm::angle(quat)},
        .axis  = glm::axis(quat),
    };
}

inline RotationAroundAxis toRotationAroundAxis(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationAroundAxis(arg); }, rotation);
}

inline RotationBase toRotationBase(const RotationBase& rotation) { return rotation; }

inline RotationBase toRotationBase(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRadians(rotation.angle).value, rotation.axis);
    const auto mat  = glm::mat3_cast(quat);
    return RotationBase{
        .right   = mat[0],
        .up      = mat[1],
        .forward = mat[2],
    };
}

inline RotationBase toRotationBase(const glm::dmat3& rotation) {
    return RotationBase{
        .right   = rotation[0],
        .up      = rotation[1],
        .forward = rotation[2],
    };
}

inline RotationBase toRotationBase(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationBase(arg); }, rotation);
}

inline glm::dmat3 toMatrix(const glm::dmat3& rotation) { return rotation; }

inline glm::dmat3 toMatrix(const RotationBase& rotation) { return glm::dmat3(rotation.right, rotation.up, rotation.forward); }

inline glm::dmat3 toMatrix(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRadians(rotation.angle).value, rotation.axis);
    return glm::mat3_cast(quat);
}

inline glm::dmat3 toMatrix(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toMatrix(arg); }, rotation);
}

}  // namespace rayx::design
