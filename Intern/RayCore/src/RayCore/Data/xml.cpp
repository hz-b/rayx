#include "xml.h"
#include <cstdio>
#include <cstring>

namespace RAYX {
    namespace xml {
        bool param(rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out) {
            for (rapidxml::xml_node<>* p = node->first_node();; p = p->next_sibling()) {
                if (strcmp(p->name(), "param")) { continue; }
                if (strcmp(p->first_attribute("id")->value(), paramname) == 0) {
                    *out = p;
                    return true;
                }
            }
            return false;
        }

        bool paramDouble(rapidxml::xml_node<>* node, const char* paramname, double* out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }

            sscanf(ref->value(), "%le", out); // TODO(rudi): error handling

            return true;
        }

        bool paramInt(rapidxml::xml_node<>* node, const char* paramname, int* out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }

            sscanf(ref->value(), "%d", out); // TODO(rudi): error handling

            return true;
        }

        bool paramStr(rapidxml::xml_node<>* node, const char* paramname, const char** out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            *out = ref->value();
            return true;
        }
    }
}