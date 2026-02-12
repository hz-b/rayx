#pragma once

#include <string>
#include <variant>

namespace rayx {

// TODO: add strings for materials consisting of multiple atomic elements

enum class Material {
#define X(elementName, atomicNumber, atomicWeight, density) elementName,
#include "materials.xmacro"
#undef X
};

}  // namespace rayx
