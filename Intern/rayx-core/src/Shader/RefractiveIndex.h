#ifndef REFRACTIVE_INDEX_H
#define REFRACTIVE_INDEX_H

#include "Adapt.h"

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

int RAYX_API getPalikEntryCount(int material);

int RAYX_API getNffEntryCount(int material);

PalikEntry RAYX_API getPalikEntry(int index, int material);

NffEntry RAYX_API getNffEntry(int index, int material);

// returns dvec2 to represent a complex number
dvec2 RAYX_API getRefractiveIndex(double energy, int material);

// returns dvec2(atomic mass, density) extracted from materials.xmacro
dvec2 RAYX_API getAtomicMassAndRho(int material);

#endif
