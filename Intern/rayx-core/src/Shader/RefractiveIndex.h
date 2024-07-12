#pragma once

#include "Common.h"
#include "Complex.h"
#include "InvocationState.h"

namespace RAYX {

/// Both of these structs are used to effectively calculate the Refractive Index of some material when hit by a photon with a particular m_energy.
/// The PalikEntry represents the the complex-valued refractive index directly as n + ik.
/// The NffEntry instead holds atomic scattering factors which can be used to derive the refractive index.

/// This struct represents one line of a .NKP file.
struct PalikEntry {
    double m_energy;
    double m_n;
    double m_k;
};

/// This struct represents one line of a .nff file.
struct NffEntry {
    double m_energy;

    // atomic scattering factors:
    double m_f1;
    double m_f2;
};

RAYX_FN_ACC int RAYX_API getPalikEntryCount(int material, InvState& inv);

RAYX_FN_ACC int RAYX_API getNffEntryCount(int material, InvState& inv);

RAYX_FN_ACC PalikEntry RAYX_API getPalikEntry(int index, int material, InvState& inv);

RAYX_FN_ACC NffEntry RAYX_API getNffEntry(int index, int material, InvState& inv);

// returns dvec2 to represent a complex number
RAYX_FN_ACC complex::Complex RAYX_API getRefractiveIndex(double energy, int material, InvState& inv);

// returns dvec2(atomic mass, density) extracted from materials.xmacro
RAYX_FN_ACC dvec2 RAYX_API getAtomicMassAndRho(int material);

} // namespace RAYX
