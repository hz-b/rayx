#pragma once

#include "Translation.h"
#include "Rotation.h"

namespace rayx::design {

struct Transform {
    Translation translation;
    Rotation rotation;
};

// struct TranslationRotation {
//     Translation translation;
//     Rotation rotation;
// };
//
// using Transform = std::variant<TranslationRotaiton, glm::dmat4>;
//
// TranslationRotation toTranslationRotation(const TranslationRotation& translationRotation) {
//     return translationRotation;
// }
//
// TranslationRotation toTranslationRotation(const glm::dmat4 mat) {
//     // TODO
// }
//
// TranslationRotation toTranslationRotation(const Transform& transform) {
//     return std::visit([] (const auto& alt) { return toTranslationRotation(alt); }, transform);
// }
//
// glm::dmat4 toMatrix(const TranslationRotation& translationRotation) {
//     const auto translation = glm::translate(glm::dmat4(1.0f), translationRotation.translation);
//     const auto rotation = translationRotation.rotation;
//     return translation * rotation;
// }
//
// glm::dmat4 toMatrix(const glm::dmat4& mat) {
//     return mat;
// }
//
// glm::dmat4 toMatrix(const Transform& transform) {
//     return std::visit([] (const auto& alt) { return toMatrix(alt); }, transform);
// }

}  // namespace rayx::design
