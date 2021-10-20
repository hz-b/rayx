#include "xml.h"
#include <cstdio>
#include <cstring>

namespace RAYX {
    namespace xml {
        bool param(rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out) {
            for (rapidxml::xml_node<>* p = node->first_node(); p; p = p->next_sibling()) {
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
            if (sscanf(ref->value(), "%le", out) != 1) { return false; }

            return true;
        }

        bool paramInt(rapidxml::xml_node<>* node, const char* paramname, int* out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            if (sscanf(ref->value(), "%d", out) != 1) { return false; }

            return true;
        }

        bool paramStr(rapidxml::xml_node<>* node, const char* paramname, const char** out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            *out = ref->value();
            return true;
        }

        bool paramDvec3(rapidxml::xml_node<>* node, const char* paramname, glm::dvec3* out) {
            rapidxml::xml_node<>* subnode;
            if (!param(node, paramname, &subnode)) { return false; }

            const char* names[3] = {"x", "y", "z"};
            double* ptrs[3] = {&out->x, &out->y, &out->z};

            for (rapidxml::xml_node<>* p = subnode->first_node(); p; p = p->next_sibling()) {
                for (uint32_t i = 0; i < 3; i++) {
                    if (strcmp(p->name(), names[i]) == 0) {
                        if (sscanf(p->value(), "%le", ptrs[i]) != 1) { return false; }
                        break;
                    }
                }
            }

            return true;
        }

        bool paramYes(rapidxml::xml_node<>* node, const char* paramname) {
            rapidxml::xml_node<>* p;
            if (!param(node, paramname, &p)) { return false; }

            return strcmp(p->first_attribute("comment")->value(), "Yes") == 0;
        }


    }
}