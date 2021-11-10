#include "Exporter.h"

#include <iostream>
#include <vector>

#include "rapidxml.hpp"

namespace RAYX {
Exporter::Exporter() {}

Exporter::~Exporter() {}

void Exporter::exportToXML() {
    std::string text = "This is a test!";
    std::vector<char> cstr(text.c_str(), text.c_str() + text.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr.data());
}
}  // namespace RAYX