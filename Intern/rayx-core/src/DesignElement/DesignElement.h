#pragma once

#include "Value.h"
#include "Shader/Element.h"

namespace RAYX{

struct DesignElement {
    Value v;
    Element compile() const;


    void setName(std::string s);
    std::string getName();

    void setWorldPosition(glm::dvec4 p);

    void setWorldOrientation(glm::dmat4x4 o);

    

};
}