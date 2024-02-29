#include "DesignSource.h"

#include "Debug/Debug.h"

namespace RAYX {


void DesignSource::setStokeslin0(double value){
    v["stokes"] = Map();
    v["stokes"]["linPol0"] = value;
}

void DesignSource::setStokeslin45(double value){
    v["stokes"]["linPol45"] = value;
}

void DesignSource::setStokescirc(double value){
    v["stokes"]["circPol"] = value;
}

glm::dvec4 DesignSource::getStokes() const {
    return glm::dvec4{v["stokes"]["linPol0"].as_double(), v["stokes"]["linPol45"].as_double(), v["stokes"]["circPol"].as_double(), 0};
}


}
