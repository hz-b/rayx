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

void DesignElement::setMisalignment(Misalignment m) {
    v["rotationXerror"] = m.m_rotationXerror.rad;
    v["rotationYerror"] = m.m_rotationYerror.rad;
    v["rotationZerror"] = m.m_rotationZerror.rad;

    v["translationXerror"] = m.m_translationXerror;
    v["translationYerror"] = m.m_translationYerror;
    v["translationZerror"] = m.m_translationZerror;
}


Misalignment DesignElement::getMisalignment() const {
    Misalignment m;
    m.m_rotationXerror.rad = v["rotationXerror"].as_double();
    m.m_rotationYerror.rad = v["rotationYerror"].as_double();
    m.m_rotationZerror.rad = v["rotationZerror"].as_double();

    m.m_translationXerror = v["translationXerror"].as_double();
    m.m_translationYerror = v["translationYerror"].as_double();
    m.m_translationZerror = v["translationZerror"].as_double();

    return m;
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


void DesignElement::setCutout(Cutout c) {
    v["geometricalShape"] = c.m_type;
    if (c.m_type == 0) {
        RectCutout rect = deserializeRect(c);
        v["CutoutWidth"] = rect.m_width;
        v["CutoutLength"] = rect.m_length;
    } else if (c.m_type == 1) {
        EllipticalCutout elli = deserializeElliptical(c);
        v["CutoutDiameterX"] = elli.m_diameter_x;
        v["CutoutDiameterZ"] = elli.m_diameter_z;
    } else if (c.m_type == 2) {
        TrapezoidCutout trapi = deserializeTrapezoid(c);
        v["CutoutWidthA"] = trapi.m_widthA;
        v["CutoutWidthB"] = trapi.m_widthB;
        v["CutoutLength"] = trapi.m_length;
    }
}
Cutout DesignElement::getCutout() const {
    Cutout c;

    c.m_type = v["geometricalShape"].as_double();

    if (c.m_type == 0) { // Rectangle
        RectCutout rect;
        rect.m_width = v["CutoutWidth"].as_double();
        rect.m_length = v["CutoutLength"].as_double();
        c = serializeRect(rect);
    } else if (c.m_type == 1) { //Ellipsoid
        EllipticalCutout elli;
        elli.m_diameter_x = v["CutoutDiameterX"].as_double();
        elli.m_diameter_z = v["CutoutDiameterZ"].as_double();
        c = serializeElliptical(elli);
    } else if (c.m_type == 2) { //Trapezoid
        TrapezoidCutout trapi;
        trapi.m_widthA = v["CutoutWidthA"].as_double();
        trapi.m_widthB = v["CutoutWidthB"].as_double();
        trapi.m_length = v["CutoutLength"].as_double();
        c = serializeTrapezoid(trapi);
    }

    return c;
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


void DesignElement::setDistancePreceding(double distance) {
    v["distancePreceding"] = distance;
}
double DesignElement::getDistancePreceding() const {
    return v["distancePreceding"].as_double();
}

void DesignElement::setTotalHeight(double height) {
    v["totalHeight"] = height;
}
double DesignElement::getTotalHeight() const {
    return v["totalHeight"].as_double();
}

void DesignElement::setOpeningShape(double shape) {
    v["openingShape"] = shape;
}
double DesignElement::getOpeningShape() const {
    return v["openingShape"].as_double();
}

void DesignElement::setOpeningWidth(double width) {
    v["openingWidth"] = width;
}
double DesignElement::getOpeningWidth() const {
    return v["openingWidth"].as_double();
}

void DesignElement::setOpeningHeight(double height) {
    v["openingHeight"] = height;
}
double DesignElement::getOpeningHeight() const {
    return v["openingHeight"].as_double();
}

void DesignElement::setCentralBeamstop(CentralBeamstop value) {
    v["centralBeamstop"] = value;
}
CentralBeamstop DesignElement::getCentralBeamstop() const {
    return v["centralBeamstop"].as_centralBeamStop();
}

void DesignElement::setStopWidth(double width) {
    v["stopWidth"] = width;
}
double DesignElement::getStopWidth() const {
    return v["stopWidth"].as_double();
}

void DesignElement::setStopHeight(double height) {
    v["stopHeight"] = height;
}
double DesignElement::getStopHeight() const {
    return v["stopHeight"].as_double();
}

void DesignElement::setTotalWidth(double width) {
    v["totalWidth"] = width;
}
double DesignElement::getTotalWidth() const {
    return v["totalWidth"].as_double();
}
}  // namespace RAYX