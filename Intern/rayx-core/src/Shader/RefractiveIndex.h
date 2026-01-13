#pragma once

#include "Complex.h"
#include "InvocationState.h"

namespace rayx {

/// Both of these structs are used to effectively calculate the Refractive Index of some material when hit by a photon with a particular m_energy.
/// The PalikEntry represents the the complex-valued refractive index directly as n + ik.
/// The NffEntry instead holds atomic scattering factors which can be used to derive the refractive index.

struct NKEntry {
    double m_energy;
    double m_n;
    double m_k;
};

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

/// This struct represents one line of a .f12 file from the cromer materialtables.
struct CromerEntry {
    double m_energy;
    double m_f1;
    double m_f2;
};

RAYX_FN_ACC int RAYX_API getPalikEntryCount(int material, const int* materialIndices);

RAYX_FN_ACC int RAYX_API getNffEntryCount(int material, const int* materialIndices);

RAYX_FN_ACC int RAYX_API getCromerEntryCount(int material, const int* materialIndices);

RAYX_FN_ACC int RAYX_API getMolecEntryCount(int material, const int* materialIndices);

RAYX_FN_ACC PalikEntry RAYX_API getPalikEntry(int index, int material, const int* materialIndices, const double* materialTable);

RAYX_FN_ACC NKEntry RAYX_API getNffEntry(int index, int material, const int* materialIndices, const double* materialTable);

RAYX_FN_ACC NKEntry RAYX_API getCromerEntry(int index, int material, const int* materialIndices, const double* materialTable);

RAYX_FN_ACC NKEntry RAYX_API getMolecEntry(int index, int material, const int* materialIndices, const double* materialTable);

// returns dvec2 to represent a complex number
RAYX_FN_ACC complex::Complex RAYX_API getRefractiveIndex(double energy, int material, const int* materialIndices, const double* materialTable);


}  // namespace rayx
