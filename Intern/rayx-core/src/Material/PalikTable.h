#pragma once

#include <string>
#include <vector>

#include "../Shared/NffPalik.h"

namespace RAYX {
/** This struct represents the contents of a .NKP file.
 *
 * Example usage:
 *
 * PalikTable cu;
 * bool success = PalikTable::load("CU", &cu); // the palik table for copper!
 *
 */
struct PalikTable {
    std::string m_element;
    std::vector<PalikEntry> m_Lines;

    /** loads the .NKP file of the element `element` and writes it's contents to
     * `out` */
    static bool load(const char* element, PalikTable* out);

    // double interpolate(double x) const;
};

}  // namespace RAYX