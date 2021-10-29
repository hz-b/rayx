#include <fstream>
#include <sstream>
#include <iostream>

#include <Data/dat.h>

namespace RAYX {
    bool DatFile::load(const char* filename, DatFile* out) {
        std::ifstream s(filename);

        std::string line;

        // line 1
        std::getline(s, out->title);

        // line 2
        std::getline(s, line);
        if (sscanf(line.c_str(), "%d %le %le %le", &out->linecount, &out->start, &out->end, &out->step) != 4) {
            std::cerr << "Failed to parse DatFile \"" << filename << "\", at line 2: \"" << line << "\"\n";
            return false;
        }
        out->lines.reserve(out->linecount);

        // line 3..EOF
        for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
            std::getline(s, line);
            if (line.empty()) { continue; }

            DatEntry e;
            
            if (sscanf(line.c_str(), "%le %le", &e.energy, &e.weight) != 2) {
                std::cerr << "Failed to parse DatFile \"" << filename << "\", at line " << lineidx << ": \"" << line << "\"\n";
                return false;
            }
            out->lines.push_back(e);
        }

        return true;
    }

    std::string DatFile::dump() {
        std::stringstream s;
        s << title << '\n';
        s << linecount << ' ' << start << ' ' << end << ' ' << step << '\n';
        for (auto line: lines) {
            s << line.energy << ' ' << line.weight << "\n";
        }

        return s.str();
    }

    double DatFile::selectEnergy(bool continuous) const {
        return 0; //TODO(rudi)
    }
}