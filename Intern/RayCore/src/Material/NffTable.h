#pragma once

#include <string>
#include <vector>

/** This struct represents one line of a .nff file.  */
struct NffEntry {
    double m_energy;
    double m_f1;
    double m_f2;
};

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
