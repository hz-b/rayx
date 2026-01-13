
#include "NffTable.h"

#include <algorithm>
#include <fstream>

#include "Debug/Debug.h"
#include "Rml/Locate.h"

namespace RAYX {

bool NffTable::load(const char* element, NffTable* out) {
    std::string elementString = element;

    std::transform(elementString.begin(), elementString.end(), elementString.begin(), [](unsigned char c) { return std::tolower(c); });

    std::filesystem::path f = ResourceHandler::getInstance().getResourcePath(std::filesystem::path("Data") / "nff" / (elementString + ".nff"));
    RAYX_VERB << "Loading NffTable from " << f;
    std::ifstream s(f);

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
#if defined(WIN32)
        if (sscanf_s(line.c_str(), "%le %le %le", &e.m_energy, &e.m_f1, &e.m_f2) != 3) {
#else
        if (sscanf(line.c_str(), "%le %le %le", &e.m_energy, &e.m_f1, &e.m_f2) != 3) {
#endif
            RAYX_WARN << "Failed to parse NffTable\"" << element << "\", at line " << lineidx << ": \"" << line << "\"";
            return false;
        }
    }

    out->m_element = element;

    return true;
}

}  // namespace RAYX