#include "Writer.h"

#include "Debug/Debug.h"

std::string RAYX_API defaultFormatString() {
    std::string output = "";
    for (auto arg : FULL_FORMAT) {
        if (output.size() > 0) {
            output += "|";
        }
        output += arg.name;
    }
    return output;
}

FormatComponent RAYX_API componentFromString(std::string comp) {
    for (auto c : FULL_FORMAT) {
        if (comp == c.name) {
            return c;
        }
    }
    RAYX_WARN << "Cannot evaluate format component \"" << comp << "\"!";
    RAYX_ERR << "A valid format would be \"" << defaultFormatString() << "\"";
    return {};
}

Format RAYX_API formatFromString(std::string s) {
    Format output = {};
    std::stringstream ss(s);
    std::string comp = "";

    while (std::getline(ss, comp, '|')) {
        output.push_back(componentFromString(comp));
    }

    return output;
}
