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

        bool paramMisalignment(rapidxml::xml_node<>* node, std::vector<double>* out) {
            *out = std::vector<double>(6, 0.f);

            rapidxml::xml_node<>* p;
            if (!param(node, "alignmentError", &p)) { return false; }

            if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
                // all misalignment-values will be left at 0 if they are missing.
                // Hence we ignore the return values of the upcoming paramDouble-calls.
                xml::paramDouble(node, "translationXerror", &((*out)[0]));
                xml::paramDouble(node, "translationYerror", &((*out)[1]));
                xml::paramDouble(node, "translationZerror", &((*out)[2]));

                xml::paramDouble(node, "rotationXerror", &((*out)[3]));
                (*out)[3] /= 1000.f; // mrad -> rad conversion

                xml::paramDouble(node, "rotationYerror", &((*out)[4]));
                (*out)[4] /= 1000.f;

                xml::paramDouble(node, "rotationZerror", &((*out)[5]));
                (*out)[5] /= 1000.f;
            }

            return true;
        }
    }
}