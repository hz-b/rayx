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

    void addBeamlineObjectFromXML(rapidxml::xml_node<>* node, Beamline* beamline) {
        const char* type = node->first_attribute("type")->value();

        if (strcmp(type, "Point Source") == 0) {
            beamline->m_LightSources.push_back(PointSource::createFromXML(node));
        } else if (strcmp(type, "ImagePlane") == 0) {
            beamline->m_OpticalElements.push_back(ImagePlane::createFromXML(node));
        } else { // TODO(rudi): extend this!
            std::cerr << "could not construct beamline object with Name: " <<  node->first_attribute("name")->value() << "; Type: " << node->first_attribute("type")->value() << '\n';
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

        for (rapidxml::xml_node<>* object = xml_beamline->first_node(); object; object = object->next_sibling()) { // Iterating through objects
            addBeamlineObjectFromXML(object, &beamline);
        }
        return beamline;
    }

} // namespace RAYX
