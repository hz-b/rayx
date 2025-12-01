#pragma once

#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace rayx {

// essentially a quaternion rotation (glm::dquat)
struct RotationAroundAxis {
    Angle angle;
    glm::dvec3 axis;
};

struct RotationBase {
    glm::dvec3 right;
    glm::dvec3 up;
    glm::dvec3 forward;
};

using Rotation = std::variant<RotationAroundAxis, RotationBase, glm::dmat3>;

RotationAroundAxis toRotationAroundAxis(const RotationAroundAxis& rotation) { return rotation; }

RotationAroundAxis toRotationAroundAxis(const RotationBase& rotation) {
    const auto quat = glm::quat_cast(glm::dmat3(rotation.right, rotation.up, rotation.forward));
    return RotationAroundAxis{
        .angle = Radians{glm::angle(quat)},
        .axis  = glm::axis(quat),
    };
}

RotationAroundAxis toRotationAroundAxis(const glm::dmat3& rotation) {
    const auto quat = glm::quat_cast(rotation);
    return RotationAroundAxis{
        .angle = Radians{glm::angle(quat)},
        .axis  = glm::axis(quat),
    };
}

RotationAroundAxis toRotationAroundAxis(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationAroundAxis(arg); }, rotation);
}

RotationBase toRotationBase(const RotationBase& rotation) { return rotation; }

RotationBase toRotationBase(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRadians(rotation.angle).value, rotation.axis);
    const auto mat  = glm::mat3_cast(quat);
    return RotationBase{
        .right   = mat[0],
        .up      = mat[1],
        .forward = mat[2],
    };
}

RotationBase toRotationBase(const glm::dmat3& rotation) {
    return RotationBase{
        .right   = rotation[0],
        .up      = rotation[1],
        .forward = rotation[2],
    };
}

RotationBase toRotationBase(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationBase(arg); }, rotation);
}

glm::dmat3 toRotationMatrix(const glm::dmat3& rotation) { return rotation; }

glm::dmat3 toRotationMatrix(const RotationBase& rotation) { return glm::dmat3(rotation.right, rotation.up, rotation.forward); }

glm::dmat3 toRotationMatrix(const RotationAroundAxis& rotation) {
    const auto quat = glm::angleAxis(toRadians(rotation.angle).value, rotation.axis);
    return glm::mat3_cast(quat);
}

glm::dmat3 toRotationMatrix(const Rotation& rotation) {
    return std::visit([](auto&& arg) { return toRotationMatrix(arg); }, rotation);
}

}  // namespace rayx
