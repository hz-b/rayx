#include "DatFile.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Debug/Debug.h"
#include "Random.h"

namespace rayx {
bool DatFile::load(const std::filesystem::path& filename, DatFile* out) {
    try {
        std::ifstream s(filename);
        RAYX_VERB << filename;
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
            RAYX_D_ERR << "Failed to parse DatFile \"" << filename << "\", at line 2: \"" << line << "\"";
            return false;
        }
        out->m_Lines.reserve(out->m_lineCount);

        // line 3..EOF
        out->m_weightSum = 0;
        for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
            if (line.empty()) { continue; }

            DatEntry e{};
#if defined(WIN32)
            if (sscanf_s(line.c_str(), "%le %le", &e.m_energy, &e.m_weight) != 2) {
#else
            if (sscanf(line.c_str(), "%le %le", &e.m_energy, &e.m_weight) != 2) {
#endif
                RAYX_D_ERR << "Failed to parse DatFile \"" << filename << "\", at line " << lineidx << ": \"" << line << "\"";
                return false;
            }
            out->m_Lines.push_back(e);
            out->m_weightSum += e.m_weight;
        }

        return true;
    } catch (const std::exception& e) {
        RAYX_D_ERR << "Exception caught while loading DatFile \"" << filename << "\": " << e.what();
        return false;
    }
}

[[maybe_unused]] std::string DatFile::dump() {
    std::stringstream s;
    s << m_title << '\n';
    s << m_lineCount << ' ' << m_start << ' ' << m_end << ' ' << m_step << '\n';
    for (auto line : m_Lines) { s << line.m_energy << ' ' << line.m_weight << "\n"; }

    return s.str();
}

}  // namespace rayx
