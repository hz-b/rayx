#pragma once

#include "Design/Rotation.h"
#include "ToModelTrait.h"

namespace rayx::detail {

template <>
struct ToModel<glm::dmat3> {
    static glm::dmat3 apply(const glm::dmat3& rotation) { return toMatrix(rotation); }
};

template <>
struct ToModel<RotationBase> {
    static glm::dmat3 apply(const RotationBase& rotation) { return toMatrix(rotation); }
};

template <>
struct ToModel<RotationAroundAxis> {
    static glm::dmat3 apply(const RotationAroundAxis& rotation) { return toMatrix(rotation); }
};

}  // namespace rayx::detail
