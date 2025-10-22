#pragma once

#include <string>
#include <vector>

// This file defines the CromerEntry.
#include "../Shader/RefractiveIndex.h"

namespace RAYX {
/** This struct represents the contents of a .f12 file.
 *
 * Example usage:
 *
 * CromerTable cu;
 * bool success = CromerTable::load("CU", &cu); // the cromer table for copper!
 *
 */
struct CromerTable {
    std::string m_element;
    std::vector<CromerEntry> m_Lines;

    /** loads the .NKP file of the element `element` and writes it's contents to
     * `out` */
    static bool load(const char* element, CromerTable* out);

    // double interpolate(double x) const;
};

}  // namespace RAYX
