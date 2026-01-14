#pragma once

namespace rayx::design {

using Translation = glm::dvec3;

glm::dmat4 toMatrix(const Translation& translation) {
    glm::dmat4 mat(1.0);
    mat[3][0] = translation.x;
    mat[3][1] = translation.y;
    mat[3][2] = translation.z;
    return mat;
}

}  // namespace rayx::design
