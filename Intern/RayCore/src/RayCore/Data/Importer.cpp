#include <fstream>
#include <sstream>

#include "rapidxml.hpp"

#include "Importer.h"

namespace RAYX
{
    Importer::Importer()
    {

    }

    Importer::~Importer()
    {

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
        rapidxml::xml_node<>* beamline = doc.first_node("lab")->first_node("beamline");

        for (rapidxml::xml_node<>* object = beamline->first_node(); object; object = object->next_sibling()) // Iterating through objects
        {
            printf("Name: %s; Type: %s \n", object->first_attribute("name")->value(), object->first_attribute("type")->value());


            for (rapidxml::xml_node<>* param = object->first_node(); param; param = param->next_sibling()) // Iterating through params
            {
                if (param->value_size() == 0) // if parameter is made up of multiple values
                {
                    printf("\tid: %s; enable: %s \n", param->first_attribute("id")->value(), param->first_attribute("enabled")->value());
                    for (rapidxml::xml_node<>* paramVal = param->first_node(); paramVal; paramVal = paramVal->next_sibling()) // Iterating through params
                    {
                        printf("\t\t%s: %s\n", paramVal->name(), paramVal->value());
                    }
                }
                else {
                    printf("\tid: %s; enable: %s; value: %s\n", param->first_attribute("id")->value(), param->first_attribute("enabled")->value(), param->value());
                }
            }
        }
    }

} // namespace RAYX
