
#include "RefractiveIndexTable.h"

#include <Debug.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "PathResolver.h"

bool RefractiveIndexTable::load(const char* element, RefractiveIndexTable* out) {
    std::string f = "Data/RefractiveIndex/" + std::string(element) + ".NKP";
    std::ifstream s(resolvePath(f));

    if (s.fail()) {
        return false;
    }

    std::string line;

    // ignore first three lines
    for (int i = 0; i < 3; i++) {
        std::getline(s, line);
    }

    // line 4..EOF
    for (uint32_t lineidx = 4; std::getline(s, line); lineidx++) {
        if (line.empty()) {
            continue;
        }

        RefractiveIndexEntry e;

        if (sscanf(line.c_str(), "%le %le %le", &e.m_energy, &e.m_n, &e.m_k) !=
            3) {
            RAYX_ERR << "Failed to parse RefractiveIndexTable\"" << element
                     << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }
        out->m_Lines.push_back(e);
    }

    out->m_element = element;

    return true;
}
