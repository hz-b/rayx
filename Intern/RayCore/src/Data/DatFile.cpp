#include <fstream>
#include <sstream>
#include <iostream>

#include <Data/DatFile.h>

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
        out->weightSum = 0;
        for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
            if (line.empty()) { continue; }

            DatEntry e;
            
            if (sscanf(line.c_str(), "%le %le", &e.energy, &e.weight) != 2) {
                std::cerr << "Failed to parse DatFile \"" << filename << "\", at line " << lineidx << ": \"" << line << "\"\n";
                return false;
            }
            out->lines.push_back(e);
            out->weightSum += e.weight;
        }

        // calculation of the expected value

        out->average = 0;
        for (auto line : out->lines) {
            out->average += line.weight / out->weightSum * line.energy;
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

    double DatFile::selectEnergy(std::mt19937& rng, bool continuous) const {
        if (continuous) {
            if (lines.size() == 1) { // weird edge case, which would crash the code below
                return lines[0].energy;
            }
            // this first rng() call will be used to find the index `idx`, s.t.
            // we will return an energy between lines[idx].energy and lines[idx+1].energy
            double percentage = ((double) rng()) / std::mt19937::max(); // in [0, 1]
            double continuousWeightSum = weightSum - lines.front().weight/2 - lines.back().weight/2;
            double w = percentage * continuousWeightSum; // in [0, continuousWeightSum]

            double counter = 0;
            uint32_t idx = 0;
            for (; idx < lines.size()-2; idx++) {
                counter += (lines[idx].weight + lines[idx+1].weight)/2;
                if (counter >= w) { break; }
            }

            // this second rng() call will be used to interpolate between lines[idx].energy and lines[idx+1].energy
            // percentage == 0 will yield lines[idx].energy, and percentage == 1 will yield lines[idx+1].energy
            percentage = ((double) rng()) / std::mt19937::max(); // in [0, 1]
            return lines[idx].energy * (1-percentage) + lines[idx+1].energy * percentage;
        } else {
            double percentage = ((double) rng()) / std::mt19937::max(); // in [0, 1]
            double w = percentage * weightSum; // in [0, weightSum]

            double counter = 0;
            for (auto e : lines) {
                counter += e.weight;
                if (counter >= w) {
                    return e.energy;
                }
            }
            return lines.back().energy;
        }
    }

    double DatFile::getAverage() const {
        return average;
    }
}