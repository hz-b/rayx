#include "XMLHelper.h"


namespace RAYX {
    namespace xml {
        bool param(rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out) {
            for (rapidxml::xml_node<>* p = node->first_node();; p = param->next_sibling()) {
                if (p->name() != "param") { continue; }
                if (p->first_attribute("id") == paramname) {
                    *out = p;
                    return true;
                }
            }
            return false;
        }

        bool param_double(rapidxml::xml_node<>* node, const char* paramname, double* out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }

            std::stringstream ss;
            ss << ref->value();
            ss >> *out;

            return true;
        }

        bool param_str(rapidxml::xml_node<>* node, const char* paramname, const char** out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            *out = ref->value();
            retrn true;
        }
    }
}