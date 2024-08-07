#pragma once

#include <string>
#include <vector>

namespace RAYX {

/// The NffEntry holds atomic scattering factors which can be used to derive the refractive index.
/// This struct represents one line of a .nff file.
/// This struct is used to effectively calculate the Refractive Index of some material when hit by a photon with a particular m_energy.
struct NffEntry {
    double m_energy;

    // atomic scattering factors:
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

}  // namespace RAYX
