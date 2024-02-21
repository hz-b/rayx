#include "DesignElement.h"

#include "Debug/Debug.h"

namespace RAYX {
Element DesignElement::compile() const {
    Element e;
    if (this->getName() == "ImagePlane") {
        e = makeImagePlane(*this);
    } else if (v["name"].as_string() == "Slit") {
        // makeSlit();
    }
    return e;
}

void DesignElement::setName(std::string s) {
    v["name"] = s;
    std::cout << s << std::endl;
}

std::string DesignElement::getName() const { return v["name"].as_string(); }

void DesignElement::setWorldPosition(glm::dvec4 p) {
    v["worldPosition"] = Map();
    v["worldPosition"]["x"] = p.x;
    v["worldPosition"]["y"] = p.y;
    v["worldPosition"]["z"] = p.z;
}

glm::dvec4 DesignElement::getWorldPosition() const {
    glm::dvec4 d;
    d[0] = v["worldPosition"]["x"].as_double();
    d[1] = v["worldPosition"]["y"].as_double();
    d[2] = v["worldPosition"]["z"].as_double();
    return d;
}

void DesignElement::setWorldOrientation(glm::dmat4x4 o) {
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

glm::dmat4x4 DesignElement::getWorldOrientation() const {
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
void DesignElement::setSlopeError(SlopeError s) {
    v["SlopeError"] = Map();
    v["SlopeError"]["slopeErrorSag"] = s.m_sag;
    v["SlopeError"]["slopeErrorMer"] = s.m_mer;
    v["SlopeError"]["thermalDistortionAmp"] = s.m_thermalDistortionAmp;
    v["SlopeError"]["thermalDistortionSigmaX"] = s.m_thermalDistortionSigmaX;
    v["SlopeError"]["thermalDistortionSigmaZ"] = s.m_thermalDistortionSigmaZ;
    v["SlopeError"]["cylindricalBowingAmp"] = s.m_cylindricalBowingAmp;
    v["SlopeError"]["cylindricalBowingRadius"] = s.m_cylindricalBowingRadius;
}

SlopeError DesignElement::getSlopeError() const {
    SlopeError s;
    s.m_sag = v["SlopeError"]["slopeErrorSag"].as_double();
    s.m_mer = v["SlopeError"]["slopeErrorMer"].as_double();
    s.m_thermalDistortionAmp = v["SlopeError"]["thermalDistortionAmp"].as_double();
    s.m_thermalDistortionSigmaX = v["SlopeError"]["thermalDistortionSigmaX"].as_double();
    s.m_thermalDistortionSigmaZ = v["SlopeError"]["thermalDistortionSigmaZ"].as_double();
    s.m_cylindricalBowingAmp = v["SlopeError"]["cylindricalBowingAmp"].as_double();
    s.m_cylindricalBowingRadius = v["SlopeError"]["cylindricalBowingRadius"].as_double();

    return s;
}


void DesignElement::setAzimuthalAngle(Rad r) {
    v["AzimuthalAngle"] = r;
}

Rad DesignElement::getAzimuthalAngle() const {
    return v["AzimuthalAngle"].as_rad();
}

void DesignElement::setMaterial(Material m) {
    v["Material"] = m;
}

Material DesignElement::getMaterial() const {
    return v["Material"].as_material();
}
}  // namespace RAYX