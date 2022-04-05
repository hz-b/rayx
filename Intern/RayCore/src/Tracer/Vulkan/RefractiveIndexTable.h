#pragma once

#include <random>
#include <string>
#include <vector>

/** This struct represents one line of a .NKP file.  */
struct RefractiveIndexEntry {
    double m_energy;
    double m_n;
    double m_k;
};

/** This struct represents the contents of a .NKP file.
 *
 * Example usage:
 *
 * RefractiveIndexTable cu;
 * bool success = RefractiveIndexTable::load("CU", &cu); // the refractive index table for copper!
 *
 */
struct RefractiveIndexTable {
    std::string m_element;
    std::vector<RefractiveIndexEntry> m_Lines;

    /** loads the .NKP file of the element `element` and writes it's contents to
     * `out` */
    static bool load(const char* element, RefractiveIndexTable* out);

    // double interpolate(double x) const;
};
