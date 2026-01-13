#include "RefractiveIndex.h"

#include "Throw.h"

namespace RAYX {

// The materialTable table consists of all the entries from the Palik & Nff tables for all materials that were loaded into the shader.
// Thus, when you want to get some concrete palik entry for a particular element, you need the materialIndices table to "know where to look" in
// materialTable:
// * materialIndices[i] is the beginning of the Palik Table in materialTable for the element with atomic number i+1, for i in [0, 132].
// * materialIndices[i+133] is the beginning of the Nff Table in materialTable for the element with atomic number i+1, for i in [0, 132].
// The `i+1` is necessary as atomic numbers start at 1 (Hydrogen), while array indices start at 0.
// See InvocationState.cpp for more information about these tables.

// If you will, the materialTable table is a "sparse" 4d-datastructure of shape (2, 133, N, 3).
// - 2 because we store a Palik-table, and an Nff-table
// - 133, because we support 133 atoms and molecules of the periodic table (from Hydrogen to Uranium)
// - N is the number of entries in a given Palik/Nff table (this number depends on the periodic element)
// - 3 because each Palik / Nff entry consists of three doubles.
// - It is a "sparse" data structure as most of the entries are actually missing, we only load the actually "used" materials into the shader, not all
// materials.
// -- Thus, we cannot index into it as materialTable[i1][i2][i3][i4] directly, but instead have to use materialIndices as described above.

// The concrete layout of materialTable and materialIndices has to be compatible with the "loadMaterialTables" function from Material.cpp
// It is responsible for creating these tables.

/// The number of palik entries we currently store for this material.
RAYX_FN_ACC
int RAYX_API getPalikEntryCount(const int material, const int* materialIndices) {
    int m = material - 1;  // in [0, 132]
    // This counts how many doubles are in between the materials index, and the
    // next index in the table. Division by 3, because each entry has 3 members
    // currently: energy, n, k, padding.
    return (materialIndices[m + 1] - materialIndices[m]) / 3;
}

/// The number of nff entries we currently store for this material.
RAYX_FN_ACC
int RAYX_API getNffEntryCount(const int material, const int* materialIndices) {
    int m = material - 1;  // in [0, 132]
    // the offset of 133 (== number of materials), skips the palik table and
    // reaches into the nff table. the rest of the logic is as above.
    return (materialIndices[133 + m + 1] - materialIndices[133 + m]) / 3;
}

/// The number of cromer entries we currently store for this material.
RAYX_FN_ACC
int RAYX_API getCromerEntryCount(const int material, const int* materialIndices) {
    int m = material - 1;  // in [0, 132]
    // the offset of 266 (== number of materials * 2), skips the palik and nff tables and
    // reaches into the cromer table. the rest of the logic is as above.
    return (materialIndices[266 + m + 1] - materialIndices[266 + m]) / 3;
}

/// The number of cromer entries we currently store for this material.
RAYX_FN_ACC
int RAYX_API getMolecEntryCount(const int material, const int* materialIndices) {
    int m = material - 1;  // in [0, 132]
    // the offset of 266 (== number of materials * 2), skips the palik and nff tables and
    // reaches into the cromer table. the rest of the logic is as above.
    RAYX_VERB << "Getting MolecEntryCount for material " << material << materialIndices[399 + m + 1] << " - " << materialIndices[399 + m];
    return (materialIndices[399 + m + 1] - materialIndices[399 + m]) / 4;
}

// Indexes into the palik table of a particular material at a given index.
RAYX_FN_ACC
PalikEntry RAYX_API getPalikEntry(int index, int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    int m = material - 1;  // in [0, 132]
    // materialIndices[m] is the start of the Palik table of material m.
    // 3*index skips 'index'-many entries, because an entry consists of 3 doubles.
    int i = materialIndices[m] + 3 * index;

    PalikEntry e;
    e.m_energy = materialTable[i];
    e.m_n = materialTable[i + 1];
    e.m_k = materialTable[i + 2];

    return e;
}

RAYX_FN_ACC
NKEntry RAYX_API getNffEntry(int index, int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    int m = material - 1;  // in [0, 132]
    // materialIndices[133+m] is the start of the Nff table of material m.
    // 3*index skips 'index'-many entries.
    int i = materialIndices[133 + m] + 3 * index;

    NKEntry e;
    e.m_energy = materialTable[i];
    e.m_n = materialTable[i + 1];
    e.m_k = materialTable[i + 2];

    return e;
}

RAYX_FN_ACC
CromerEntry RAYX_API getCromerEntry(int index, int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    int m = material - 1;  // in [0, 132]
    // materialIndices[266+m] is the start of the Cromer table of material m.
    // 3*index skips 'index'-many entries.
    int i = materialIndices[266 + m] + 3 * index;

    CromerEntry e;
    e.m_energy = materialTable[i];
    e.m_f1 = materialTable[i + 1];
    e.m_f2 = materialTable[i + 2];

    return e;
}

RAYX_FN_ACC
NKEntry RAYX_API getMolecEntry(int index, int material, const int* __restrict materialIndices, const double* __restrict materialTable) {
    int m = material - 1;  // in [0, 132]
    // materialIndices[399+m] is the start of the Molec table of material m.
    // 4*index skips 'index'-many entries.
    int i = materialIndices[399 + m] + 4 * index;

    NKEntry e;
    e.m_energy = materialTable[i];
    e.m_n = materialTable[i + 1];
    e.m_k = materialTable[i + 2];

    return e;
}

// returns dvec2 to represent a complex number
RAYX_FN_ACC
complex::Complex RAYX_API getRefractiveIndex(double energy, int material, const int* __restrict materialIndices,
                                             const double* __restrict materialTable) {
    
    RAYX_VERB << "Getting refractive index for material " << material << " at energy " << energy;

    if (material == -1) {  // vacuum
        return complex::Complex(1., 0.);
    }

    // out of range check
    if (material < 1 || material > 140) {
        _throw("getRefractiveIndex material out of range!");
        return complex::Complex(-1.0, -1.0);
    }

    //check if material is an atom < 92 
    if (material <= 92) {
            // try to get refractive index using Palik table
        if (getPalikEntryCount(material, materialIndices) > 0) {           // don't try binary search if there are 0 entries!
            int low = 0;                                                   // <= energy
            int high = getPalikEntryCount(material, materialIndices) - 1;  // >= energy

            PalikEntry low_entry = getPalikEntry(low, material, materialIndices, materialTable);
            PalikEntry high_entry = getPalikEntry(high, material, materialIndices, materialTable);

            if (low_entry.m_energy <= energy && energy <= high_entry.m_energy) {  // if 'energy' is in range of tha PalikTable
                // binary search
                while (high - low > 1) {
                    int center = (low + high) / 2;
                    PalikEntry center_entry = getPalikEntry(center, material, materialIndices, materialTable);
                    if (energy < center_entry.m_energy) {
                        high = center;
                    } else {
                        low = center;
                    }
                }

                PalikEntry entry = getPalikEntry(low, material, materialIndices, materialTable);
                //RAYX_VERB << "Using PalikEntry: energy=" << entry.m_energy << " n=" << entry.m_n << " k=" << entry.m_k;
                return complex::Complex(entry.m_n, entry.m_k);
            }
        }

        // get refractive index with Nff table
        if (getNffEntryCount(material, materialIndices) > 0) {           // don't try binary search if there are 0 entries!
            int low = 0;                                                 // <= energy
            int high = getNffEntryCount(material, materialIndices) - 1;  // >= energy

            // binary search
            while (high - low > 1) {
                int center = (low + high) / 2;
                NKEntry center_entry = getNffEntry(center, material, materialIndices, materialTable);
                if (energy < center_entry.m_energy) {
                    high = center;
                } else {
                    low = center;
                }
            }
            auto entry = getNffEntry(low, material, materialIndices, materialTable);
            //RAYX_VERB << "Using NffEntry: energy=" << entry.m_energy << " n=" << entry.m_n << " k=" << entry.m_k;
            return complex::Complex(entry.m_n, entry.m_k);
        }

        // get refractive index with Cromer table
        if (getCromerEntryCount(material, materialIndices) > 0) {           // don't try binary search if there are 0 entries!
            int low = 0;                                                    // <= energy
            int high = getCromerEntryCount(material, materialIndices) - 1;  // >= energy

            // binary search
            while (high - low > 1) {
                int center = (low + high) / 2;
                CromerEntry center_entry = getCromerEntry(center, material, materialIndices, materialTable);
                if (energy < center_entry.m_energy) {
                    high = center;
                } else {
                    low = center;
                }
            }

            // compute n, k from the Cromer data.
            glm::dvec2 massAndRho = getAtomicMassAndRho(material);
            double mass = massAndRho.x;
            double rho = massAndRho.y;

            CromerEntry entry = getCromerEntry(low, material, materialIndices, materialTable);
            double e = entry.m_energy;
            double n = 1 - (415.252 * rho * entry.m_f1) / (e * e * mass);
            double k = (415.252 * rho * entry.m_f2) / (e * e * mass);
            //RAYX_VERB << "Using CromerEntry: energy=" << entry.m_energy << " f1=" << entry.m_f1 << " f2=" << entry.m_f2;
            return complex::Complex(n, k);
        }    
    } else if (material > 92 && material <= 140) {
        //molecules are not supported yet
        RAYX_VERB << "checking for molecule refractive index table for material " << material;
        if(getMolecEntryCount(material, materialIndices) > 0) {
            int low = 0;                                                    // <= energy
            int high = getMolecEntryCount(material, materialIndices) - 1;  // >= energy

            // binary search
            while (high - low > 1) {
                int center = (low + high) / 2;
                NKEntry center_entry = getMolecEntry(center, material, materialIndices, materialTable);
                if (energy < center_entry.m_energy) {
                    high = center;
                } else {
                    low = center;
                }
            }

            NKEntry entry = getMolecEntry(low, material, materialIndices, materialTable);
            //RAYX_VERB << "Using MolecEntry: energy=" << entry.m_energy << " n=" << entry.m_n << " k=" << entry.m_k;
            return complex::Complex(entry.m_n, entry.m_k);
        }
    }

    _throw("getRefractiveIndex: no matching entry found!");
    return complex::Complex(-1.0, -1.0);
}
}  // namespace RAYX