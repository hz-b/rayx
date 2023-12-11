#include "DatFile.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Debug/Debug.h"
#include "Random.h"

namespace RAYX {
bool DatFile::load(const std::filesystem::path& filename, DatFile* out) {
    if (!std::filesystem::exists(filename)) {
        RAYX_ERR << "DatFile " << filename << " doesn't exist!";
        return false;
    }
    std::ifstream s(filename);

    std::string line;

    // line 1
    std::getline(s, out->m_title);

    // line 2
    std::getline(s, line);
#if defined(WIN32)
    if (sscanf_s(line.c_str(), "%u %le %le %le", &out->m_lineCount, &out->m_start, &out->m_end, &out->m_step) != 4) {
#else
    if (sscanf(line.c_str(), "%u %le %le %le", &out->m_lineCount, &out->m_start, &out->m_end, &out->m_step) != 4) {
#endif
        RAYX_ERR << "Failed to parse DatFile \"" << filename << "\", at line 2: \"" << line << "\"";
        return false;
    }
    out->m_Lines.reserve(out->m_lineCount);

    // line 3..EOF
    out->m_weightSum = 0;
    for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
        if (line.empty()) {
            continue;
        }

        DatEntry e{};
#if defined(WIN32)
        if (sscanf_s(line.c_str(), "%le %le", &e.m_energy, &e.m_weight) != 2) {
#else
        if (sscanf(line.c_str(), "%le %le", &e.m_energy, &e.m_weight) != 2) {
#endif
            RAYX_ERR << "Failed to parse DatFile \"" << filename << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }
        out->m_Lines.push_back(e);
        out->m_weightSum += e.m_weight;
    }

    // calculation of the expected value

    out->m_average = 0;
    for (auto _line : out->m_Lines) {
        out->m_average += _line.m_weight / out->m_weightSum * _line.m_energy;
    }

    return true;
}

[[maybe_unused]] std::string DatFile::dump() {
    std::stringstream s;
    s << m_title << '\n';
    s << m_lineCount << ' ' << m_start << ' ' << m_end << ' ' << m_step << '\n';
    for (auto line : m_Lines) {
        s << line.m_energy << ' ' << line.m_weight << "\n";
    }

    return s.str();
}

double DatFile::selectEnergy() const {
    // runs either continuous Energydistribution from DataFile or just the specific energies
    // provisionally set to true because EnergyDistibution ended support for this choice
    // TODO: Fanny find a way to get a choise for DataFile Distribution back
    if (m_continuous) {
        if (m_Lines.size() == 1) {  // weird edge case, which would crash the code below
            return m_Lines[0].m_energy;
        }
        // find the index `idx`, s.t.
        // we will return an energy between lines[idx].energy and
        // lines[idx+1].energy
        double continuousWeightSum = m_weightSum - m_Lines.front().m_weight / 2 - m_Lines.back().m_weight / 2;
        double w = randomDoubleInRange(0, continuousWeightSum);

        double counter = 0;
        uint32_t idx = 0;

        for (; idx < m_Lines.size() - 2; idx++) {
            counter += (m_Lines[idx].m_weight + m_Lines[idx + 1].m_weight) / 2;
            if (counter >= w) {
                break;
            }
        }

        // interpolate between lines[idx].energy and lines[idx+1].energy
        return randomDoubleInRange(m_Lines[idx].m_energy, m_Lines[idx + 1].m_energy);
    } else {
        double w = randomDoubleInRange(0, m_weightSum);

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
