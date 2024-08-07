#pragma once

#include "Complex.h"
#include "InvocationState.h"
#include "Material/NffTable.h"
#include "Material/PalikTable.h"

namespace RAYX {

RAYX_FN_ACC int RAYX_API getPalikEntryCount(int material, InvState& inv);

RAYX_FN_ACC int RAYX_API getNffEntryCount(int material, InvState& inv);

RAYX_FN_ACC PalikEntry RAYX_API getPalikEntry(int index, int material, InvState& inv);

RAYX_FN_ACC NffEntry RAYX_API getNffEntry(int index, int material, InvState& inv);

// returns dvec2 to represent a complex number
RAYX_FN_ACC complex::Complex RAYX_API getRefractiveIndex(double energy, int material, InvState& inv);

// returns dvec2(atomic mass, density) extracted from materials.xmacro
RAYX_FN_ACC glm::dvec2 RAYX_API getAtomicMassAndRho(int material);

}  // namespace RAYX
