#include <fstream>
#include <sstream>

#include "rapidxml.hpp"

#include "Importer.h"
#include <string.h>

namespace RAYX
{
    Importer::Importer()
    {

    }

    Importer::~Importer()
    {

    }

    // TODO(rudi): use std::optional<std::shared_ptr<BeamlineObject>> when updating to C++17
    std::shared_ptr<OpticalElement> createBeamlineObjectFromXML(rapidxml::xml_node<>* node) {
        const char* type = node->first_attribute("type")->value();
        if (strcmp(type, "Point Source") ==  0) {
            // TODO(rudi)
            // return std::static_pointer_cast<OpticalBeamElement>(PointSource::createFromXML(node));

        // TODO(rudi):
        // } else if (strcmp(type, ...) ==  0) { ... }

        } else {
            return nullptr;
        }
    }

    Beamline Importer::importBeamline() {
        // first implementation: stringstreams are slow; this might need optimization
        std::ifstream t("test.rml");
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string test = buffer.str();
        std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
        rapidxml::xml_document<> doc;
        doc.parse<0>(cstr.data());

        std::cout << "Version: " << doc.first_node("lab")->first_node("version")->value() << std::endl;
        rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");

        Beamline beamline;

        for (rapidxml::xml_node<>* object = xml_beamline->first_node(); object; object = object->next_sibling()) // Iterating through objects
        {
            printf("Name: %s; Type: %s \n", object->first_attribute("name")->value(), object->first_attribute("type")->value());

            const auto obj = createBeamlineObjectFromXML(object);
            if (obj) {
                // TODO(rudi)
                // beamline.m_Objects.push_back(obj);
            } else {
                std::cerr << "could not construct beamline object:\n";
                std::cerr << "Name: " <<  object->first_attribute("name")->value() << "; Type: " << object->first_attribute("type")->value() << '\n';
            }
        }

        return Beamline();
    }

} // namespace RAYX
