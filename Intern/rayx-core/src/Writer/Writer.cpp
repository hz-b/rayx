#include "Writer.h"

#include <sstream>

#include "Debug/Debug.h"

namespace RAYX {

std::string defaultFormatString() {
    std::string output;
    for (auto arg : FULL_FORMAT) {
        if (!output.empty()) {
            output += "|";
        }
        output += arg.name;
    }
    return output;
}

FormatComponent componentFromString(const std::string& comp) {
    for (auto c : FULL_FORMAT) {
        if (comp == c.name) {
            return c;
        }
    }
    RAYX_WARN << "Cannot evaluate format component \"" << comp << "\"!";
    RAYX_ERR << "A valid format would be \"" << defaultFormatString() << "\"";
    return {};
}

Format formatFromString(const std::string& s) {
    Format output = {};
    std::stringstream ss(s);
    std::string comp;

    while (std::getline(ss, comp, '|')) {
        output.push_back(componentFromString(comp));
    }

    return output;
}

}  // namespace RAYX
