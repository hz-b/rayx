#pragma once

// TODO(rudi): move this out of the Vulkan Tracer, doesn't belong here anymore.

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

// the following functions load the tables, and grant you a pointer to them.
// the tables will be written to the nkp and nkpIdx buffers of shader.comp
const std::vector<double>* getMaterialTable();
const std::vector<int>* getMaterialIndexTable();