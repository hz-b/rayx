
#include "MolecTable.h"

#include <algorithm>
#include <fstream>

#include "Debug/Debug.h"
#include "Rml/Locate.h"

namespace RAYX {

bool MolecTable::load(const char* element, MolecTable* out) {
    std::string elementString = element;

    std::transform(elementString.begin(), elementString.end(), elementString.begin(), [](unsigned char c) { return std::toupper(c); });

    std::filesystem::path f = ResourceHandler::getInstance().getResourcePath(std::filesystem::path("Data") / "MOLEC" / (elementString + ".NKM"));
    RAYX_VERB << "Loading MolecTable from " << f;
    std::ifstream s(f);

    if (s.fail()) {
        return false;
    }

    std::string line;

    // ignore first line
    std::getline(s, line);
    std::getline(s, line);

    // line 2..EOF
    for (uint32_t lineidx = 3; std::getline(s, line); lineidx++) {
        if (line.empty()) {
            continue;
        }

        NKEntry e{};
#if defined(WIN32)
        if (sscanf_s(line.c_str(), "%le %le %le", &e.m_energy, &e.m_n, &e.m_k) != 3) {
#else
        if (sscanf(line.c_str(), "%le %le %le", &e.m_energy, &e.m_n, &e.m_k) != 3) {
#endif
            RAYX_WARN << "Failed to parse MolecTable\"" << element << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }
        out->m_Lines.push_back(e);
    }

    out->m_element = element;

    return true;
}

}  // namespace RAYX