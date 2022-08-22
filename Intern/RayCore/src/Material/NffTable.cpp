
#include "NffTable.h"

#include <Debug.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "PathResolver.h"

bool NffTable::load(const char* element, NffTable* out) {
    std::string elementString = element;

    std::transform(elementString.begin(), elementString.end(),
                   elementString.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::string f = "Data/nff/" + elementString + ".nff";
    std::ifstream s(resolvePath(f));

    if (s.fail()) {
        return false;
    }

    std::string line;

    // ignore first line
    std::getline(s, line);

    // line 2..EOF
    for (uint32_t lineidx = 2; std::getline(s, line); lineidx++) {
        if (line.empty()) {
            continue;
        }

        NffEntry e{};

        if (sscanf(line.c_str(), "%le %le %le", &e.m_energy, &e.m_f1,
                   &e.m_f2) != 3) {
            RAYX_ERR << "Failed to parse NffTable\"" << element
                     << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }
        out->m_Lines.push_back(e);
    }

    out->m_element = element;

    return true;
}
