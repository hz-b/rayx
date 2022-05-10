#include "DatFile.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Debug.h"

namespace RAYX {
bool DatFile::load(std::filesystem::path filename, DatFile* out) {
    std::ifstream s(filename);

    std::string line;

    // line 1
    std::getline(s, out->m_title);

    // line 2
    std::getline(s, line);
    if (sscanf(line.c_str(), "%u %le %le %le", &out->m_linecount, &out->m_start,
               &out->m_end, &out->m_step) != 4) {
        RAYX_ERR << "Failed to parse DatFile \"" << filename
                 << "\", at line 2: \"" << line << "\"";
        return false;
    }
    out->m_Lines.reserve(out->m_linecount);

    // line 3..EOF
    out->m_weightSum = 0;
    for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
        if (line.empty()) {
            continue;
        }

        DatEntry e;

        if (sscanf(line.c_str(), "%le %le", &e.m_energy, &e.m_weight) != 2) {
            RAYX_ERR << "Failed to parse DatFile \"" << filename
                     << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }
        out->m_Lines.push_back(e);
        out->m_weightSum += e.m_weight;
    }

    // calculation of the expected value

    out->m_average = 0;
    for (auto line : out->m_Lines) {
        out->m_average += line.m_weight / out->m_weightSum * line.m_energy;
    }

    return true;
}

std::string DatFile::dump() {
    std::stringstream s;
    s << m_title << '\n';
    s << m_linecount << ' ' << m_start << ' ' << m_end << ' ' << m_step << '\n';
    for (auto line : m_Lines) {
        s << line.m_energy << ' ' << line.m_weight << "\n";
    }

    return s.str();
}

double DatFile::selectEnergy(std::mt19937& rng, bool continuous) const {
    if (continuous) {
        if (m_Lines.size() ==
            1) {  // weird edge case, which would crash the code below
            return m_Lines[0].m_energy;
        }
        // this first rng() call will be used to find the index `idx`, s.t.
        // we will return an energy between lines[idx].energy and
        // lines[idx+1].energy
        double percentage = ((double)rng()) / std::mt19937::max();  // in [0, 1]
        double continuousWeightSum = m_weightSum -
                                     m_Lines.front().m_weight / 2 -
                                     m_Lines.back().m_weight / 2;
        double w =
            percentage * continuousWeightSum;  // in [0, continuousWeightSum]

        double counter = 0;
        uint32_t idx = 0;
        for (; idx < m_Lines.size() - 2; idx++) {
            counter += (m_Lines[idx].m_weight + m_Lines[idx + 1].m_weight) / 2;
            if (counter >= w) {
                break;
            }
        }

        // this second rng() call will be used to interpolate between
        // lines[idx].energy and lines[idx+1].energy percentage == 0 will yield
        // lines[idx].energy, and percentage == 1 will yield lines[idx+1].energy
        percentage = ((double)rng()) / std::mt19937::max();  // in [0, 1]
        return m_Lines[idx].m_energy * (1 - percentage) +
               m_Lines[idx + 1].m_energy * percentage;
    } else {
        double percentage = ((double)rng()) / std::mt19937::max();  // in [0, 1]
        double w = percentage * m_weightSum;  // in [0, weightSum]

        double counter = 0;
        for (auto e : m_Lines) {
            counter += e.m_weight;
            if (counter >= w) {
                return e.m_energy;
            }
        }
        return m_Lines.back().m_energy;
    }
}

double DatFile::getAverage() const { return m_average; }
}  // namespace RAYX
