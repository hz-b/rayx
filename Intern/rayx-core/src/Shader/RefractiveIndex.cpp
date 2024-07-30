#include "RefractiveIndex.h"

#include "Throw.h"

namespace RAYX {

// The inv.mat table consists of all the entries from the Palik & Nff tables for all materials that were loaded into the shader.
// Thus, when you want to get some concrete palik entry for a particular element, you need the inv.matIdx table to "know where to look" in inv.mat:
// * inv.matIdx[i] is the beginning of the Palik Table in inv.mat for the element with atomic number i+1, for i in [0, 91].
// * inv.matIdx[i+92] is the beginning of the Nff Table in inv.mat for the element with atomic number i+1, for i in [0, 91].
// The `i+1` is necessary as atomic numbers start at 1 (Hydrogen), while array indices start at 0.
// See InvocationState.cpp for more information about these tables.

// If you will, the inv.mat table is a "sparse" 4d-datastructure of shape (2, 92, N, 3).
// - 2 because we store a Palik-table, and an Nff-table
// - 92, because we support 92 elements of the periodic table (from Hydrogen to Uranium)
// - N is the number of entries in a given Palik/Nff table (this number depends on the periodic element)
// - 3 because each Palik / Nff entry consists of three doubles.
// - It is a "sparse" data structure as most of the entries are actually missing, we only load the actually "used" materials into the shader, not all
// materials.
// -- Thus, we cannot index into it as inv.mat[i1][i2][i3][i4] directly, but instead have to use inv.matIdx as described above.

// The concrete layout of inv.mat and inv.matIdx has to be compatible with the "loadMaterialTables" function from Material.cpp
// It is responsible for creating these tables.

/// The number of palik entries we currently store for this material.
RAYX_FN_ACC
int RAYX_API getPalikEntryCount(int material, InvState& inv) {
    int m = material - 1;  // in [0, 91]
    // This counts how many doubles are in between the materials index, and the
    // next index in the table. Division by 3, because each entry has 3 members
    // currently: energy, n, k, padding.
    return (inv.matIdx[m + 1] - inv.matIdx[m]) / 3;
}

/// The number of nff entries we currently store for this material.
RAYX_FN_ACC
int RAYX_API getNffEntryCount(int material, InvState& inv) {
    int m = material - 1;  // in [0, 91]
    // the offset of 92 (== number of materials), skips the palik table and
    // reaches into the nff table. the rest of the logic is as above.
    return (inv.matIdx[92 + m + 1] - inv.matIdx[92 + m]) / 3;
}

// Indexes into the palik table of a particular material at a given index.
RAYX_FN_ACC
PalikEntry RAYX_API getPalikEntry(int index, int material, InvState& inv) {
    int m = material - 1;  // in [0, 91]
    // inv.matIdx[m] is the start of the Palik table of material m.
    // 3*index skips 'index'-many entries, because an entry consists of 3 doubles.
    int i = inv.matIdx[m] + 3 * index;

    PalikEntry e;
    e.m_energy = inv.mat[i];
    e.m_n = inv.mat[i + 1];
    e.m_k = inv.mat[i + 2];

    return e;
}

RAYX_FN_ACC
NffEntry RAYX_API getNffEntry(int index, int material, InvState& inv) {
    int m = material - 1;  // in [0, 91]
    // inv.matIdx[92+m] is the start of the Nff table of material m.
    // 3*index skips 'index'-many entries.
    int i = inv.matIdx[92 + m] + 3 * index;

    NffEntry e;
    e.m_energy = inv.mat[i];
    e.m_f1 = inv.mat[i + 1];
    e.m_f2 = inv.mat[i + 2];

    return e;
}

// returns dvec2 to represent a complex number
RAYX_FN_ACC
complex::Complex RAYX_API getRefractiveIndex(double energy, int material, InvState& inv) {
    if (material == -1) {  // vacuum
        return complex::Complex(1., 0.);
    }

    // out of range check
    if (material < 1 || material > 92) {
        _throw("getRefractiveIndex material out of range!");
        return complex::Complex(-1.0, -1.0);
    }

    // try to get refractive index using Palik table
    if (getPalikEntryCount(material, inv) > 0) {           // don't try binary search if there are 0 entries!
        int low = 0;                                       // <= energy
        int high = getPalikEntryCount(material, inv) - 1;  // >= energy

        PalikEntry low_entry = getPalikEntry(low, material, inv);
        PalikEntry high_entry = getPalikEntry(high, material, inv);

        if (low_entry.m_energy <= energy && energy <= high_entry.m_energy) {  // if 'energy' is in range of tha PalikTable
            // binary search
            while (high - low > 1) {
                int center = (low + high) / 2;
                PalikEntry center_entry = getPalikEntry(center, material, inv);
                if (energy < center_entry.m_energy) {
                    high = center;
                } else {
                    low = center;
                }
            }

            PalikEntry entry = getPalikEntry(low, material, inv);
            return complex::Complex(entry.m_n, entry.m_k);
        }
    }

    // get refractive index with Nff table
    if (getNffEntryCount(material, inv) > 0) {           // don't try binary search if there are 0 entries!
        int low = 0;                                     // <= energy
        int high = getNffEntryCount(material, inv) - 1;  // >= energy

        // binary search
        while (high - low > 1) {
            int center = (low + high) / 2;
            NffEntry center_entry = getNffEntry(center, material, inv);
            if (energy < center_entry.m_energy) {
                high = center;
            } else {
                low = center;
            }
        }

        // compute n, k from the Nff data.
        glm::dvec2 massAndRho = getAtomicMassAndRho(material);
        double mass = massAndRho.x;
        double rho = massAndRho.y;

        NffEntry entry = getNffEntry(low, material, inv);
        double e = entry.m_energy;
        double n = 1 - (415.252 * rho * entry.m_f1) / (e * e * mass);
        double k = (415.252 * rho * entry.m_f2) / (e * e * mass);

        return complex::Complex(n, k);
    }

    _throw("getRefractiveIndex: no matching entry found!");
    return complex::Complex(-1.0, -1.0);
}

// returns dvec2(atomic mass, density) extracted from materials.xmacro
RAYX_FN_ACC
glm::dvec2 RAYX_API getAtomicMassAndRho(int material) {
    // This is an "X-Macro", see https://en.wikipedia.org/wiki/X_macro
    // It allows us to generate a `case` for each material in the materials.xmacro file.
    // The `case` matches upon the atomic number of the element, and returns the atomic mass and density as specified in the materials.xmacro file.
    switch (material) {
#define X(e, z, a, rho) \
    case z:             \
        return glm::dvec2(a, rho);
#include "../Material/materials.xmacro"
#undef X
    }
    _throw("invalid material in getAtomicMassAndRho");
    return glm::dvec2(0.0, 0.0);
}

}  // namespace RAYX
