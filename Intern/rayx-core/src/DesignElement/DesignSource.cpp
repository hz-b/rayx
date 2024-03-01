#include "DesignSource.h"

#include "Debug/Debug.h"
#include "Beamline/Objects/Objects.h"

namespace RAYX {


std::vector<Ray> DesignSource::compile(int i) const {
    std::vector<Ray> ray;
    if (getName() == "Point Source") {
        //ray = LightSource::getRays(1);
        std::cout << getName() << std::endl;
    }
}


void DesignSource::setName(std::string s) { 
    v["name"] = s; 
    std::cout << v["name"].as_string() << std::endl;
}
std::string DesignSource::getName() const { return v["name"].as_string(); }


void DesignSource::setWorldPosition(glm::dvec4 p) {
    v["worldPosition"] = Map();
    v["worldPosition"]["x"] = p.x;
    v["worldPosition"]["y"] = p.y;
    v["worldPosition"]["z"] = p.z;
}

glm::dvec4 DesignSource::getWorldPosition() const {
    glm::dvec4 d;
    d[0] = v["worldPosition"]["x"].as_double();
    d[1] = v["worldPosition"]["y"].as_double();
    d[2] = v["worldPosition"]["z"].as_double();
    return d;
}

void DesignSource::setWorldOrientation(glm::dmat4x4 o) {
    v["worldXDirection"] = Map();
    v["worldXDirection"]["x"] = o[0][0];
    v["worldXDirection"]["y"] = o[0][1];
    v["worldXDirection"]["z"] = o[0][2];

    v["worldYDirection"] = Map();
    v["worldYDirection"]["x"] = o[1][0];
    v["worldYDirection"]["y"] = o[1][1];
    v["worldYDirection"]["z"] = o[1][2];

    v["worldZDirection"] = Map();
    v["worldZDirection"]["x"] = o[2][0];
    v["worldZDirection"]["y"] = o[2][1];
    v["worldZDirection"]["z"] = o[2][2];
}

glm::dmat4x4 DesignSource::getWorldOrientation() const {
    glm::dmat4x4 o;

    o[0][0] = v["worldXDirection"]["x"].as_double();
    o[0][1] = v["worldXDirection"]["y"].as_double();
    o[0][2] = v["worldXDirection"]["z"].as_double();

    o[1][0] = v["worldYDirection"]["x"].as_double();
    o[1][1] = v["worldYDirection"]["y"].as_double();
    o[1][2] = v["worldYDirection"]["z"].as_double();

    o[2][0] = v["worldZDirection"]["x"].as_double();
    o[2][1] = v["worldZDirection"]["y"].as_double();
    o[2][2] = v["worldZDirection"]["z"].as_double();

    return o;
}

void DesignSource::setMisalignment(Misalignment m) {
    v["rotationXerror"] = m.m_rotationXerror.rad;
    v["rotationYerror"] = m.m_rotationYerror.rad;
    v["rotationZerror"] = m.m_rotationZerror.rad;

    v["translationXerror"] = m.m_translationXerror;
    v["translationYerror"] = m.m_translationYerror;
    v["translationZerror"] = m.m_translationZerror;
}

Misalignment DesignSource::getMisalignment() const {
    Misalignment m;
    m.m_rotationXerror.rad = v["rotationXerror"].as_double();
    m.m_rotationYerror.rad = v["rotationYerror"].as_double();
    m.m_rotationZerror.rad = v["rotationZerror"].as_double();

    m.m_translationXerror = v["translationXerror"].as_double();
    m.m_translationYerror = v["translationYerror"].as_double();
    m.m_translationZerror = v["translationZerror"].as_double();

    return m;
}

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
    return glm::dvec4{1, v["stokes"]["linPol0"].as_double(), v["stokes"]["linPol45"].as_double(), v["stokes"]["circPol"].as_double()};
}


void DesignSource::setWidthDist(SourceDist value) { v["widthDist"] = value; }
SourceDist DesignSource::getWidthDist() const { return v["widthDist"].as_sourceDist(); }

void DesignSource::setHeightDist(SourceDist value) { v["heightDist"] = value; }
SourceDist DesignSource::getHeightDist() const { return v["heightDist"].as_sourceDist(); }

void DesignSource::setHorDist(SourceDist value) { v["horDist"] = value; }
SourceDist DesignSource::getHorDist() const { return v["horDist"].as_sourceDist(); }

void DesignSource::setVerDist(SourceDist value) { v["verDist"] = value; }
SourceDist DesignSource::getVerDist() const { return v["verDist"].as_sourceDist(); }

void DesignSource::setHorDivergence(double value) { v["horDivergence"] = value; }
double DesignSource::getHorDivergence() const { return v["horDivergence"].as_double(); }

void DesignSource::setVerDivergence(double value) { v["verDivergence"] = value; }
double DesignSource::getVerDivergence() const { return v["verDivergence"].as_double(); }

void DesignSource::setSourceDepth(double value) { v["sourceDepth"] = value; }
double DesignSource::getSourceDepth() const { return v["sourceDepth"].as_double(); }

void DesignSource::setSourceHeight(double value) { v["sourceHeight"] = value; }
double DesignSource::getSourceHeight() const { return v["sourceHeight"].as_double(); }

void DesignSource::setSourceWidth(double value) { v["sourceWidth"] = value; }
double DesignSource::getSourceWidth() const { return v["sourceWidth"].as_double(); }

void DesignSource::setEnergySpread(double value) { v["energySpread"] = value; }
void DesignSource::setEnergyDistributionType(EnergyDistributionType value) { v["energyDistributionType"] = value; }
void DesignSource::setEnergyDistributionFile(std::string value) { v["photonEnergyDistributionFile"] = value; }
void DesignSource::setEnergySpreadType(SpreadType value) { v["energyDistribution"] = value; }

EnergyDistribution DesignSource::getEnergyDistribution() const { 
    EnergyDistribution en;
    SpreadType spreadType = v["energySpread"].as_energySpreadType();

    return en;
}

void DesignSource::setNumberOfRays(double value) { v["numberOfRays"] = value; }
double DesignSource::getNumberOfRays() const { return v["numberOfRays"].as_double(); }

void DesignSource::setEnergy(double value) { v["energy"] = value; }
double DesignSource::getEnergy() const { return v["energy"].as_double(); }

}
