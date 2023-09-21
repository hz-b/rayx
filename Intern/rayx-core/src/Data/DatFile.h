#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "Core.h"

namespace RAYX {
/** This struct represents one line of a .DAT file.  */
struct DatEntry {
    float m_energy;
    float m_weight;
};

/** This struct represents the contents of a .DAT file. */
struct DatFile {
    std::string m_title;
    uint32_t m_linecount;
    float m_start;
    float m_end;
    float m_step;

    float m_weightSum;
    float m_average;

    std::vector<DatEntry> m_Lines;

    /** loads the .DAT file `filename` and writes it's contents to `out` */
    static bool load(const std::filesystem::path& filename, DatFile* out);

    /** creates a valid .DAT file from this struct (may be used for testing) */
    [[maybe_unused]] std::string dump();

    /** samples from the distribution given by the .DAT file */
    float selectEnergy(bool continuous) const;

    /** yields the expected value of the underlying distribution */
    float getAverage() const;
};
}  // namespace RAYX
