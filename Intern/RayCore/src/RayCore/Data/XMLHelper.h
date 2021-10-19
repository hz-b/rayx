#pragma once

#include "rapidxml.hpp"

namespace RAYX {
    namespace xml {
        // these functions return whether they were successfull, and return their output in the argument `out`
        bool param(rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out);
        bool param_double(rapidxml::xml_node<>* node, const char* paramname, double* out);
        bool param_int(rapidxml::xml_node<>* node, const char* paramname, int* out);
        bool param_str(rapidxml::xml_node<>* node, const char* paramname, const char** out);
    }
}
