#pragma once

#include "Design/Rotation.h"
#include "ToModel.h"

namespace rayx::detail {

inline glm::dmat3 toModel(const glm::dmat3& rotation) { return toMatrix(rotation); }
inline glm::dmat3 toModel(const RotationBase& rotation) { return toMatrix(rotation); }
inline glm::dmat3 toModel(const RotationAroundAxis& rotation) { return toMatrix(rotation); }

}
