#pragma once

#include <string>
#include <vector>

// This file defines the NffEntry.
#include "../Shader/RefractiveIndex.h"

namespace RAYX {
/** This struct represents the contents of a .NKP file.
 *
 * Example usage:
 *
 * NffTable cu;
 * bool success = NffTable::load("CU", &cu); // the refractive index
 * table for copper!
 *
 */
struct NffTable {
    std::string m_element;
    std::vector<NffEntry> m_Lines;

    /** loads the .NKP file of the element `element` and writes it's contents to
     * `out` */
    static bool load(const char* element, NffTable* out);

    // double interpolate(double x) const;
};

}  // namespace RAYX
