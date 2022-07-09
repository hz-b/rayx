#pragma once

#include <array>
#include <vector>

/**
 * This enum class is the c++ perspective on the Materials stored in
 * "materials.xmacro".
 * Whenever you want to add a material, just add a line in materials.xmacro:
 * everything else is done automagically. This makes heavy use of X Macros, see
 * https://en.wikipedia.org/wiki/X_Macro
 **/
enum class Material {
    REFLECTIVE = -2,
    VACUUM = -1,

// Material = 0 is not used yet

#define X(e, z, a, rho) e = z,
#include "materials.xmacro"
#undef X
};

/** returns whether a Material with the name `matname` has been found.
 * it will be stored in `out`.
 **/
bool materialFromString(const char* matname, Material* out);

struct MaterialTables {
    std::vector<double> materialTable;
    std::vector<int> indexTable;
};

// the following function loads the Palik & Nff tables.
// the tables will later be written to the mat and matIdx buffers of shader.comp
MaterialTables loadMaterialTables(std::array<bool, 92> relevantMaterials);