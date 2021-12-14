#pragma once

#include <vector>

enum class Material {
#define X(e) e,
#include "materials.xmacro"
#undef X
};

void fillMaterialTables(std::vector<int>* materialIndex,
                        std::vector<double>* material);