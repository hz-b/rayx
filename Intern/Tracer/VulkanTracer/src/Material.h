#pragma once

#include <vector>

enum class Material {
#define X(e) e,
#include "materials.xmacro"
#undef X
};

const std::vector<double>* getMaterialTable();
const std::vector<int>* getMaterialIndexTable();