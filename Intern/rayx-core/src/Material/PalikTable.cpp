#include "PalikTable.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "CanonicalizePath.h"
#include "Debug/Debug.h"

bool PalikTable::load(const char* element, PalikTable* out) {
    std::string elementString = element;
    std::transform(elementString.begin(), elementString.end(), elementString.begin(), [](unsigned char c) { return std::toupper(c); });

    std::string f = "Data/PALIK/" + elementString + ".NKP";
    std::ifstream s(canonicalizeRepositoryPath(f));

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

        PalikEntry e{};
        double temp_energy, temp_n, temp_k;  // Temporary double variables
        std::istringstream iss(line);        // Create a string stream from the line

        // Read from the string stream into temporary double variables
        if (!(iss >> temp_energy >> temp_n >> temp_k)) {
            RAYX_ERR << "Failed to parse PalikTable \"" << element << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }

        // Explicitly cast to float and store in struct
        e.m_energy = static_cast<float>(temp_energy);
        e.m_n = static_cast<float>(temp_n);
        e.m_k = static_cast<float>(temp_k);

        out->m_Lines.push_back(e);
    }

    out->m_element = element;

    return true;
}
