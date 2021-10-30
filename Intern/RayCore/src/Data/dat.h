#pragma once

#include <vector>
#include <string>
#include <random>

namespace RAYX {
    /** This struct represents one line of a .DAT file.  */
    struct DatEntry {
        double energy;
        double weight;
    };

    /** This struct represents the contents of a .DAT file. */
    struct DatFile {
        std::string title;
        uint32_t linecount;
        double start;
        double end;
        double step;

        std::vector<DatEntry> lines;

        /** loads the .DAT file `filename` and writes it's contents to `out` */
        static bool load(const char* filename, DatFile* out);

        /** creates a valid .DAT file from this struct (may be used for testing) */
        std::string dump();

        double selectEnergy(std::mt19937& rng, bool continuous) const;
    };
}