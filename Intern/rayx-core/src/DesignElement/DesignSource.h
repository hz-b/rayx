#pragma once

#include "Beamline/Objects/Objects.h"
#include "Beamline/LightSource.h"
#include "Value.h"

namespace RAYX {

struct DesignSource {
    Value v;
    Element compile() const;

    void setStokeslin0(double value);
    void setStokeslin45(double value);
    void setStokescirc(double value);
    glm::dvec4 getStokes() const;

};
}
