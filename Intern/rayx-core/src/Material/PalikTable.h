#pragma once

#include <string>
#include <vector>

namespace RAYX {

/// The PalikEntry represents the the complex-valued refractive index directly as n + ik.
/// This struct represents one line of a .NKP file.
/// This struct is used to effectively calculate the Refractive Index of some material when hit by a photon with a particular m_energy.
struct PalikEntry {
    double m_energy;
    double m_n;
    double m_k;
};

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
