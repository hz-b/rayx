
#include <Data/PalikTable.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace RAYX {
bool PalikTable::load(const char* element, PalikTable* out) {
    std::string f = "../../Data/PALIK/";  // TODO(rudi): make paths more robust!
    f += element;
    f += ".NKP";
    std::ifstream s(f);

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

        PalikEntry e;

        if (sscanf(line.c_str(), "%le %le %le", &e.m_energy, &e.m_n, &e.m_k) !=
            3) {
            std::cerr << "Failed to parse PalikTable \"" << element
                      << "\", at line " << lineidx << ": \"" << line << "\"\n";
            return false;
        }
        out->m_Lines.push_back(e);
    }

    return true;
}
}