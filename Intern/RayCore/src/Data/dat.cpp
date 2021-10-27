#include <fstream>
#include <sstream>
#include <iostream>

#include <Data/dat.h>

namespace RAYX {
    bool DatFile::load(const char* filename, DatFile* out) {
        std::ifstream s(filename);

        std::string line;

        // line 1
        std::getline(s, line);
        if (line[0] != '\"' || line[line.length()-1] != '\"') {
            std::cerr << "Failed to parse DatFile \"" << filename << "\", at line 1: \"" << line << "\"\n";
            return false;
        }
        out->title = line.substr(1, line.length()-2);

        // line 2
        std::getline(s, line);
        if (sscanf(line.c_str(), "%d\t%le\t%le\t%le", &out->linecount, &out->start, &out->end, &out->step) != 4) {
            std::cerr << "Failed to parse DatFile \"" << filename << "\", at line 2: \"" << line << "\"\n";
            return false;
        }
        out->lines.reserve(out->linecount);

        // line 3..EOF
        for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
            std::getline(s, line);
            if (line.empty()) { continue; }

            DatEntry e;
            
            if (sscanf(line.c_str(), "%le\t%le.", &e.energy, &e.weight) != 2) {
                std::cerr << "Failed to parse DatFile \"" << filename << "\", at line " << lineidx << ": \"" << line << "\"\n";
                return false;
            }
            out->lines.push_back(e);
        }

        return true;
    }
}