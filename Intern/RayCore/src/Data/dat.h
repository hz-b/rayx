#pragma once

#include <vector>
#include <string>

namespace RAYX {
    struct DatEntry {
        double energy;
        double weight;
    };

    struct DatFile {
        std::string title;
        uint32_t linecount;
        double start;
        double end;
        double step;

        std::vector<DatEntry> lines;

        // loads the .DAT file `filename` and writes it's contents to out
        static bool load(const char* filename, DatFile* out);
    };
}