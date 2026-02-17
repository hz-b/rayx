
#include "rml.h"

#include <cstring>
#include <stdexcept>

#include "IO/Debug.h"

namespace rayx {





// This is the central function to load RML files.
Beamline importBeamline(const std::filesystem::path& filepath) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    // first implementation: stringstreams are slow; this might need
    // optimization
    RAYX_VERB << "import beamline file: " << filepath << "";

    // load the RML file to a string.
    std::ifstream t(filepath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string test = buffer.str();

    if (test.empty()) { RAYX_EXIT << "error: could not open beamline file! (or it was just empty)"; }

    // load the RML string into the rapid XML parser library.
    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr.data());

    RAYX_VERB << "RML Version: " << doc.first_node("lab")->first_node("version")->value();
    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");

    Group root;

    // go through all objects of the file, and parse them.
    // The group context stores the set of group in which the algorithm currently "is".
    // For each group we call handleObjectCollection recursively, and push the group onto the group context stack.
    handleObjectCollection(xml_beamline, root, filepath);

    return root;
}

}
