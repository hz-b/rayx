#pragma once

#include <string>
#include <vector>

// This file defines the MolecEntry.
#include "../Shader/RefractiveIndex.h"

namespace RAYX {
/** This struct represents the contents of a .NKP file.
 *
 * Example usage:
 *
 * Molec SiC2;
 * bool success = MolecTable::load("SiC2", &csic2); // the refractive index
 * table for this molecule!
 *
 */
struct MolecTable {
    std::string m_element;
    std::vector<NKEntry> m_Lines;

    /** loads the .NKP file of the element `element` and writes it's contents to
     * `out` */
    static bool load(const char* element, MolecTable* out);
    // double interpolate(double x) const;
};

}  // namespace RAYX
