#include "RefractiveIndex.h"

// returns dvec2(atomic mass, density) extracted from materials.xmacro
RAYX_FUNC
dvec2 RAYX_API getAtomicMassAndRho(int material) {
    // This is an "X-Macro", see https://en.wikipedia.org/wiki/X_macro
    // It allows us to generate a `case` for each material in the materials.xmacro file.
    // The `case` matches upon the atomic number of the element, and returns the atomic mass and density as specified in the materials.xmacro file.
    switch (material) {
#define X(e, z, a, rho) \
    case z:             \
        return dvec2(a, rho);
#include "../Material/materials.xmacro"
#undef X
    }
    _throw("invalid material in getAtomicMassAndRho");
    return dvec2(0.0, 0.0);
}


